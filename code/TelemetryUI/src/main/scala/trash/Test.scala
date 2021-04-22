package trash

import com.fazecast.jSerialComm.{SerialPort, SerialPortDataListener, SerialPortEvent}

object Test extends App {
  println(SerialPort.getCommPorts.mkString(", "))


  val port = SerialPort.getCommPort("ttyUSB0")
  port.setParity(SerialPort.NO_PARITY)
  port.setNumStopBits(1)
  port.setNumDataBits(8)
  port.setBaudRate(115200)
  port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0)
  port.openPort()

  port.addDataListener(new SerialPortDataListener() {
    override def getListeningEvents: Int = SerialPort.LISTENING_EVENT_DATA_AVAILABLE

    def serialEvent(event: SerialPortEvent): Unit = {
      if (event.getEventType != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) return
      val newData = new Array[Byte](port.bytesAvailable)
      val numRead = port.readBytes(newData, newData.length)
      println("Read " + numRead + s" bytes: ${newData.map(_.asInstanceOf[Char]).mkString("(", ", ", ")")}")
      println()
    }
  })
}
