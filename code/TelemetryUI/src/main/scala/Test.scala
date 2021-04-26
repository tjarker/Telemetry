import com.fazecast.jSerialComm._
import telemetryui.types.{CanFrame, TimeStamp}

import scala.io.StdIn._
import java.util.Date
import java.util.concurrent.{ConcurrentLinkedQueue, SynchronousQueue}
import scala.collection.mutable

object Test extends App {

  val port = SerialPort.getCommPort("ttyACM0")
  port.setBaudRate(921600)
  port.setNumDataBits(8)
  port.setNumStopBits(1)
  port.setParity(0)
  port.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 0, 0)
  port.openPort()

  import com.fazecast.jSerialComm.SerialPort
  import com.fazecast.jSerialComm.SerialPortDataListener

  val queue = new ConcurrentLinkedQueue[Array[Byte]]()

  port.addDataListener(new SerialPortMessageListener() {

    override def getMessageDelimiter(): Array[Byte] = "\n".getBytes()

    override def getListeningEvents: Int = SerialPort.LISTENING_EVENT_DATA_AVAILABLE

    def serialEvent(event: SerialPortEvent): Unit = {
      if (event.getEventType != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) return
      val newData = new Array[Byte](port.bytesAvailable)
      port.readBytes(newData, newData.length)
      queue.add(newData)
      //println("Put: "+newData.map(_.toChar).mkString(""))
    }

    override def delimiterIndicatesEndOfMessage(): Boolean = true
  })


  val thread = new Thread{
    var running = true
    override def run(): Unit = {
      var openBrackets = 0
      val str = new StringBuilder
      while(running){
        if(!queue.isEmpty){
          val bytes = queue.poll()
          bytes.foreach { byte =>
            val b = byte.toChar
            if(b != '\n'){
              str.append(b)
              //println(str.toString())
              if(b == '{') {
                openBrackets += 1
                //println(s"opened bracket => $openBrackets")
              }
              else if(b == '}') {
                if(openBrackets == 0) {
                  //println("closed bracket without being open")
                  str.clear()
                }
                else {
                  openBrackets -= 1
                  //println(s"closed bracket => $openBrackets")
                  if(openBrackets == 0){
                    //println("closed final bracket")
                    var canFrame: CanFrame = null
                    try{
                      canFrame = CanFrame(str.toString)
                      println("Is:\t\t"+CanFrame(str.toString))
                      //println("Should be:\t"+CanFrame(0x25F,false,4,0x0A0000FF,TimeStamp(4,4,4)).toString)
                      if(CanFrame(str.toString).toString != CanFrame(0x25F,false,4,0x0A0000FF,TimeStamp(4,4,4)).toString) println("!!!!!!!!!! Error")
                    }catch{
                      case e: org.json4s.MappingException =>
                        println("failed")
                      case e: Throwable => e.printStackTrace()
                    }

                    str.clear()
                  }
                }
              } else {
                if(str.size == 1) {
                  //println("clear due to wrong start")
                  str.clear()
                } else {
                  //println("all fine")
                }
              }
            }

          }
          //println(str)
          //println(bytes.map(_.toChar).mkString(""))
        }
      }

    }
  }
  thread.setDaemon(true)
  thread.start()

  val thread2 = new Thread{
    var runni = true
    override def run(): Unit = {
      val bytes = "Hello!".getBytes
      while(runni){
        //println("Sending")
        val msg = CanFrame(0x25F,false,4,0x0A0000FF,TimeStamp(4,4,4)).toByteArray
        //println(msg.mkString(", "))
        println(msg.map(_.toHexString).mkString(", "))
        port.writeBytes(msg,msg.length)
        Thread.sleep(0,1000)
      }

    }
  }
  thread2.setDaemon(true)
  thread2.start()

  readLine()
  thread.running = false
  thread2.runni = false
  port.closePort()


}
