package telemetryui.serial

import com.fazecast.jSerialComm.{SerialPort, SerialPortEvent, SerialPortMessageListener}


import java.util.concurrent.ConcurrentLinkedQueue

class SerialPortListener(port: SerialPort, queue: ConcurrentLinkedQueue[Array[Byte]]) extends SerialPortMessageListener {

  override def getMessageDelimiter: Array[Byte] = "\n".getBytes()

  override def delimiterIndicatesEndOfMessage(): Boolean = true

  override def getListeningEvents: Int = SerialPort.LISTENING_EVENT_DATA_AVAILABLE

  def serialEvent(event: SerialPortEvent): Unit = {
    if (event.getEventType != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) return
    val newData = new Array[Byte](port.bytesAvailable)
    port.readBytes(newData, newData.length)
    synchronized(println(newData.map(_.toChar).mkString("")))
    queue.add(newData)
  }
}