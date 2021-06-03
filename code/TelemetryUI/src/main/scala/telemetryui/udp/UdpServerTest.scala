package telemetryui.udp

import telemetryui.types.{CanFrame, TimeStamp}

import java.net.{DatagramPacket, DatagramSocket, InetAddress}

object UdpServerTest extends App {
  (new UdpServerTest).run()
}

class UdpServerTest extends Thread {

  val socket = new DatagramSocket(4445)
  var buf = new Array[Byte](16)

  var running = true

  override def run(): Unit = {
    while (running) {
      val rPacket = new DatagramPacket(buf,buf.length)
      socket.receive(rPacket)
      val address = rPacket.getAddress
      val port = rPacket.getPort
      val received = new String(rPacket.getData,0,rPacket.getLength)
      if(received.equals("start")){
        println("Starting...")
        for(i <- 0 until 10){
          buf = CanFrame(101,false,4,i,TimeStamp(10,8,i)).toByteArray
          val packet = new DatagramPacket(buf,buf.length,address,port)
          println("sent...")
          println(socket.isClosed)
          println(socket.isConnected)
          socket.send(packet)
          Thread.sleep(1000)
        }
      }

    }
    socket.close()
  }
}
