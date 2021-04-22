package telemetryui.serial

import com.fazecast.jSerialComm.{SerialPort, SerialPortDataListener, SerialPortEvent}
import org.json4s.DefaultFormats
import org.json4s.jackson.JsonMethods.parse
import telemetryui.types.CanFrame

class SerialPortListener(port: SerialPort, listener: Seq[CanFrame => Unit]) extends SerialPortDataListener {

  val str = new StringBuilder
  implicit val formats = DefaultFormats

  override def getListeningEvents: Int = {
    return SerialPort.LISTENING_EVENT_DATA_AVAILABLE
  }

  override def serialEvent(event: SerialPortEvent): Unit = {
    if(event.getEventType != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) return


    val newData = new Array[Byte](port.bytesAvailable())
    val numread = port.readBytes(newData,newData.length)
    val newString = newData.map(_.toChar).mkString
    println(s"Serial event: $newString")
    for(ch <- newData.slice(0,numread)){
      str.append(ch.toChar)

      if(str.toString.startsWith("<s>") && str.toString.endsWith("<e>")){

        val json = str.toString.slice(3,str.size-3)

        val canFrame = parse(json).extract[CanFrame]

        listener.foreach(_(canFrame))

        str.clear()
      }
      if(str.size > 3 && !str.toString.startsWith("<s>")) {
        println("did not start correctly")
        str.clear()
      }
      if(str.toString.sliding(3).count(_ == "<s>")>1) {
        println("multiple starts")
        str.clear()
      }
    }
  }
}