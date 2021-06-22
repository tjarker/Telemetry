package telemetryui.serial

import com.fazecast.jSerialComm.SerialPort
import telemetryui.types.CMD.RECEIVED_CAN
import telemetryui.types.{CanFrame, TelemetryMessage}
import java.util.concurrent.Semaphore

import java.util.concurrent.ConcurrentLinkedQueue

/**
 * Thread responsible for broadcasting commands, messages and errors
 *
 * Can be used to send a telemetry message to the SCU using [[send]]
 *
 * takes Telemetry messages formatted as JSON strings as input via serial
 *
 * outputs Telemetry messages as bytes. See [[CanFrame.toByteArray]] for the byte layout
 *
 */

class SerialWorker(port: SerialPort,
                   canListener: Seq[CanFrame => Unit],
                   cmdListener: Seq[Int => Unit],
                   errListener: Seq[() => Unit]
                  ) extends Thread {

  private val serialMutex = new Semaphore(1)

  this.setDaemon(true)

  // queue used for passing raw data from the event driven routine to a processing routine
  private val queue = new ConcurrentLinkedQueue[Array[Byte]]()

  // class assembling the json string with basic error checking
  private val assembler = new JsonAssembler

  // flag used to stop the thread
  private var running = true
  def isRunning: Boolean = running
  def quit(): Unit = {
    running = false
    serialMutex.acquire()
    port.removeDataListener()
    port.closePort()
    serialMutex.release()
  }

  override def run(): Unit = {

    port.addDataListener(new SerialPortListener(port,serialMutex,queue))

    while(running){

      if(!queue.isEmpty){
        queue.poll().foreach { b: Byte =>
          assembler.add(b)
          if(assembler.hasMessage) {
            val msg = assembler.getMessage
            cmdListener.foreach(f => synchronized{f(msg.cmd)})
            if(msg.cmd == RECEIVED_CAN) canListener.foreach(f => synchronized{f(msg.can.get)})
          }
          if(assembler.hadError) errListener.foreach(f => synchronized{f()})
        }
      }

    }
  }

  def send(msg: TelemetryMessage): Unit = {
    serialMutex.acquireUninterruptibly()
    val bytes = msg.toByteArray
    port.writeBytes(bytes,bytes.length)
    serialMutex.release()
  }
}
