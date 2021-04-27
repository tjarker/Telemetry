package telemetryui.types

case class TimeStamp(hour: Int, minute: Int, second: Int){
  override def toString: String = s"%02d:%02d:%02d".format(hour,minute,second)
  def toByteArray: Array[Byte] = Array(second,minute,second).map(_.toByte)
}

object TimeStamp {
  def apply(): TimeStamp = {
    val now = new java.util.Date()
    TimeStamp(now.getHours,now.getMinutes,now.getSeconds)
  }
}