package telemetryui.types

import org.json4s.DefaultFormats
import org.json4s.jackson.JsonMethods.parse

object CMD {
  val RECEIVED_CAN = 0
  val BROADCAST_CAN = 1
  val START_LOGGING = 2
  val STOP_LOGGING = 3
  val START_STREAMING = 4
  val STOP_STREAMING = 5
}

case class TelemetryMessage(cmd: Int, can: Option[CanFrame]){
  def toByteArray: Array[Byte] = Array(cmd.toByte) ++ can.getOrElse(CanFrame(0,false,0,0,TimeStamp(0,0,0))).toByteArray
}
object TelemetryMessage {
  def apply(json: String): TelemetryMessage = {
    implicit val formats = DefaultFormats
    parse(json).extract[TelemetryMessage]
  }
}