import com.fazecast.jSerialComm._
import telemetryui.serial.SerialWorker
import telemetryui.types.{CanFrame, TimeStamp}

import scala.io.StdIn._
import java.util.Date
import java.util.concurrent.{ConcurrentLinkedQueue, SynchronousQueue}
import scala.collection.mutable

object Test2 extends App {

  val port = SerialPort.getCommPort("ttyACM0")
  port.setBaudRate(921600)
  port.setNumDataBits(8)
  port.setNumStopBits(1)
  port.setParity(0)
  port.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 0, 0)
  port.openPort()

  val serialWorker = new SerialWorker(port,Seq(println),Seq(),Seq(() => println("Error!")))
  serialWorker.start()

  val thread2 = new Thread{
    var runni = true
    override def run(): Unit = {
      val bytes = "Hello!".getBytes
      while(runni){
        //println("Sending")
        val msg = CanFrame(0x25F,false,4,0x0A0000FF,TimeStamp(4,4,4)).toByteArray
        //println(msg.mkString(", "))
        //println(msg.map("%x".format(_)).mkString(", "))
        port.writeBytes(msg,msg.length)
        Thread.sleep(0,10000)
      }

    }
  }
  thread2.setDaemon(true)
  thread2.start()

  readLine()
  serialWorker.quit()
  thread2.runni = false


}