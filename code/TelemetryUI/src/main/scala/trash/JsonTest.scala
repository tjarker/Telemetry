package trash

import org.json4s.DefaultFormats
import org.json4s.jackson.JsonMethods.parse
import org.json4s.jackson.Serialization.write
import telemetryui.types.{CanFrame, TimeStamp}

object JsonTest extends App {

  case class Command(cmdID: Int)
  case class Message(cmd: Command, msg: CanFrame)



  val json = parse("""{"id":123,"rtr":false,"len":2,"data":321,"stamp":{"hour":2,"minute":3,"second":12}}""")
  implicit val formats = DefaultFormats
  val canFrame = json.extract[CanFrame]

  println(canFrame)
  println(write(canFrame))

  println(write(Message(null,CanFrame(12,false,2,12,TimeStamp(2,2,2)))))
}
