package telemetryui.types

import org.json4s.DefaultFormats
import org.json4s.jackson.JsonMethods.parse

object CMD {
  val RECEIVED_CAN = 0
  val BROADCAST_CAN = 1
}

case class TelemetryMessage(cmd: Int, can: CanFrame){
  def toByteArray: Array[Byte] = Array(cmd.toByte) ++ can.toByteArray
}
object TelemetryMessage {
  def apply(json: String): TelemetryMessage = {
    implicit val formats = DefaultFormats
    parse(json).extract[TelemetryMessage]
  }
}