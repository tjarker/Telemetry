package trash

import org.json4s.DefaultFormats
import org.json4s.jackson.JsonMethods.parse
import org.json4s.jackson.Serialization.write
import telemetryui.types.CanFrame

object JsonTest extends App {
  val json = parse("""{"id":123,"len":2,"data":321}""")
  implicit val formats = DefaultFormats
  val canFrame = json.extract[CanFrame]
  println(canFrame)
  println(write(canFrame))
}
