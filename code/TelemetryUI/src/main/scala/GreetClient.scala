import java.io.{BufferedReader, InputStreamReader, PrintWriter}
import java.net._

object GreetClient extends App {
  val client = new GreetClient("127.0.0.1", 6666)
  while (true) {
    val response = client.sendMessage("hello server")
    println(response)
  }
}

class GreetClient(ip: String, port: Int) {
  val clientSocket = new Socket(ip, port)
  val out = new PrintWriter(clientSocket.getOutputStream, true)
  val in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream))

  def sendMessage(msg: String): String = {
    out.println(msg)
    val resp = in.readLine
    resp
  }

  def stopConnection(): Unit = {
    in.close()
    out.close()
    clientSocket.close()
  }
}