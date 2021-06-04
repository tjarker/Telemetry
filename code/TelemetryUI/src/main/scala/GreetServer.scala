import java.io.BufferedReader
import java.io.PrintWriter
import java.net.ServerSocket
import scala.tools.jline_embedded.internal.InputStreamReader


object GreetServer {
  def main(args: Array[String]): Unit = {
    val server = new GreetServer(6666)
  }
}

class GreetServer(port: Int) {
  val serverSocket = new ServerSocket(port)
  val clientSocket = serverSocket.accept
  val out = new PrintWriter(clientSocket.getOutputStream, true)
  val in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream))
  while(!clientSocket.isClosed) {

    val greeting = ""//in.readLine
    if ("hello server" == greeting) out.println("hello client")
    else out.println("unrecognised greeting")

  }
  in.close()
  out.close()
  clientSocket.close()

  serverSocket.close()

}