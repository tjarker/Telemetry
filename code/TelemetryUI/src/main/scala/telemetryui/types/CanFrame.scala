package telemetryui.types

case class CanFrame(id: BigInt, len: Int, data: BigInt) {
  override def toString: String = s"($id:$len:$data)"
  def toByteArray : Array[Byte] = {
    id.toByteArray.padTo(4,0.toByte) ++ Array(len.toByte) ++ data.toByteArray.padTo(8,0.toByte).reverse
  }
}
