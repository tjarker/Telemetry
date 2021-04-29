package telemetryui.serial

import com.fazecast.jSerialComm.SerialPort
import telemetryui.types.CMD.RECEIVED_CAN
import telemetryui.types.CanFrame

import java.util.concurrent.ConcurrentLinkedQueue

class SerialWorker(port: SerialPort,
                   canListener: Seq[CanFrame => Unit],
                   cmdListener: Seq[Int => Unit],
                   errListener: Seq[() => Unit]
                  ) extends Thread {

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

    port.addDataListener(new SerialPortListener(port,queue))

    while(running){

      if(!queue.isEmpty){
        queue.poll().foreach { b: Byte =>
          assembler.add(b)
          if(assembler.hasMessage) {
            val msg = assembler.getMessage
            cmdListener.foreach(_(msg.cmd))
            if(msg.cmd == RECEIVED_CAN) canListener.foreach(_(msg.can))
          }
          if(assembler.hadError) errListener.foreach(_())
        }
      }

    }
  }
}
