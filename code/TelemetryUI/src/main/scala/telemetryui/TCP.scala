package telemetryui


import java.io._
import java.net._
import java.util.Scanner


object TCP extends App {
  val server = new ServerSocket(8888,5,InetAddress.getByName("127.0.0.1"))
  println("asd")

  while(true){
    val socket = server.accept()
    println(s"New connection at ${server.getInetAddress}")
    val out = new PrintWriter(socket.getOutputStream, true)
    out.print("Hello World!")
    socket.close()
    println("done")
  }
}

