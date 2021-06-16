package telemetryui.udp

import telemetryui.types.{CanFrame, TimeStamp}

import java.net.{DatagramPacket, DatagramSocket, SocketException}

class UdpServer extends Thread {
  val socket = new DatagramSocket(4445)

  var listenerPort = socket.getPort
  var listenerAddress = socket.getLocalAddress

  var hasListener = false

  var running = true

  override def run(): Unit = {
    val buf = new Array[Byte](16)
    val rPacket = new DatagramPacket(buf,buf.length)

    while (running) {
      synchronized{
        try{
          socket.receive(rPacket)
        } catch {
          case e: SocketException =>
        }
      }
      val received = new String(rPacket.getData,0,rPacket.getLength)
      received match {
        case "start" =>
          println("Starting UDP stream")
          listenerAddress = rPacket.getAddress
          listenerPort = rPacket.getPort
          hasListener = true
        case "end" =>
          println("Stopping UDP stream")
          hasListener = false
        case _ =>
      }

    }
  }
  def close(): Unit = {
    running = false
    socket.close()
  }
  def broadcastCanMessage(msg: CanFrame): Unit = {
    if(hasListener){
      val buf = msg.toByteArray.slice(0,16)
      val packet = new DatagramPacket(buf,buf.length,listenerAddress,listenerPort)
      synchronized{
        socket.send(packet)
      }
    }
  }
}
