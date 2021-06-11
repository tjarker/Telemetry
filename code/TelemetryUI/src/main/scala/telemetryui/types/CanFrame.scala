package telemetryui.types

import org.json4s.DefaultFormats
import org.json4s.jackson.JsonMethods.parse
import math.pow

case class CanFrame(id: BigInt, rtr: Boolean, len: Int, data: BigInt, stamp: TimeStamp) {
  override def toString: String = s"($id: [$len]$data at $stamp)"
  def toByteArray : Array[Byte] = {
    //TODO: padTo seems to create arrays that vary in size
    println(data.toByteArray.reverse.padTo(8,0.toByte).mkString(", "))
    (id.toByteArray.reverse.padTo(2,0.toByte) ++ Array((if(rtr)1 else 0).toByte) ++ Array(len.toByte) ++
      (if((data & 0x8000000000000000L) == 0) data.toByteArray.reverse.padTo(8,0.toByte) else data.toByteArray.drop(1).reverse.padTo(8,0.toByte)) ++
      stamp.toByteArray).padTo(31,0.toByte)
  }
}

object CanFrame {
  def apply(): CanFrame = {
    val rand = scala.util.Random
    CanFrame(rand.nextInt(pow(2,11).toInt),rand.nextBoolean(),rand.nextInt(8),0xFFFFFFFFFFFFFFFFL-1,TimeStamp())
  }
}
