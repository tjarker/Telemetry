package telemetryui.types

case class TimeStamp(hour: Int, minute: Int, second: Int){
  override def toString: String = s"$hour:$minute:$second"
}

object TimeStamp {
  def apply(): TimeStamp = {
    val now = new java.util.Date()
    TimeStamp(now.getHours,now.getMinutes,now.getSeconds)
  }
}