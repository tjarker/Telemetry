package telemetryui.types

import org.json4s.DefaultFormats
import org.json4s.jackson.JsonMethods.parse


case class CanFrame(id: BigInt, rtr: Boolean, len: Int, data: BigInt, stamp: TimeStamp) {
  override def toString: String = s"($id: [$len]$data at $stamp)"
  def toByteArray : Array[Byte] = {
    //TODO: padTo seems to create arrays that vary in size
    id.toByteArray.padTo(2,0.toByte).reverse ++ Array((if(rtr)1 else 0).toByte) ++ Array(len.toByte) ++ data.toByteArray.reverse.padTo(8,0.toByte) ++
      stamp.toByteArray
  }
}

object CanFrame {
  def apply(): CanFrame = {
    val rand = scala.util.Random
    CanFrame(rand.nextInt(1000),rand.nextBoolean(),rand.nextInt(8),rand.nextInt(Int.MaxValue),TimeStamp())
  }
}

