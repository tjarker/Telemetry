package telemetryui.serial

import com.fazecast.jSerialComm.SerialPort
import telemetryui.types.CMD.RECEIVED_CAN
import telemetryui.types.{CanFrame, TelemetryMessage}
import java.util.concurrent.Semaphore

import java.util.concurrent.ConcurrentLinkedQueue

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
    port.closePort()
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
