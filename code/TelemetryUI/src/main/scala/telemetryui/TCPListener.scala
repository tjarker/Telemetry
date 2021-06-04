package telemetryui

import java.net.Socket
import java.io.BufferedReader
import scala.tools.jline_embedded.internal.InputStreamReader

object TCPListener extends App{
  val socket = new Socket("127.0.0.1",8888)
  val in = new BufferedReader(new InputStreamReader(socket.getInputStream))
  println(in.readLine())
}
