package telemetryui.serial

import com.fazecast.jSerialComm.{SerialPort, SerialPortEvent, SerialPortMessageListener}

import java.util.concurrent.{ConcurrentLinkedQueue, Semaphore}

class SerialPortListener(port: SerialPort, serialMutex: Semaphore, queue: ConcurrentLinkedQueue[Array[Byte]]) extends SerialPortMessageListener {

  override def getMessageDelimiter: Array[Byte] = "\n".getBytes()

  override def delimiterIndicatesEndOfMessage(): Boolean = true

  override def getListeningEvents: Int = SerialPort.LISTENING_EVENT_DATA_AVAILABLE

  def serialEvent(event: SerialPortEvent): Unit = {
    if (event.getEventType != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) return
    var newData: Option[Array[Byte]] = None
    serialMutex.acquireUninterruptibly()
    newData = Some(new Array[Byte](port.bytesAvailable))
    port.readBytes(newData.get, newData.get.length)
    serialMutex.release()
    println(newData.get.map(_.toChar).mkString(""))
    queue.add(newData.get)
  }
}