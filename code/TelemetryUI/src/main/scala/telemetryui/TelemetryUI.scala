package telemetryui

import com.fazecast.jSerialComm.SerialPort
import org.json4s.DefaultFormats
import org.json4s.jackson.Serialization
import telemetryui.components.{CanFrameForm, CanFrameLabel, CommandButton, SerialPortSelector}
import telemetryui.serial.SerialWorker
import telemetryui.types.CMD.BROADCAST_CAN
import telemetryui.types.{CanFrame, TelemetryMessage}
import telemetryui.udp.UdpServer
import telemetryui.util.Timer

import java.awt.Dimension
import javax.swing.UIManager
import scala.swing.Swing.EmptyBorder
import scala.swing._
import scala.swing.event.ButtonClicked
import scala.sys.exit

object TelemetryUI extends SimpleSwingApplication {
  implicit val formats = DefaultFormats

  var port: Option[SerialPort] = None
  var serialWorker: Option[SerialWorker] = None

  val canLbl = new CanFrameLabel("Last Received CAN Frame")
  val canForm = new CanFrameForm("Send CAN Frame", { canFrame =>
    println(s"Sending Frame: $canFrame")
    serialWorker.get.send(TelemetryMessage(BROADCAST_CAN, canFrame))
  })

  lazy val ui = new BoxPanel(Orientation.Vertical) {
    border = EmptyBorder(10, 10, 10, 10)
    contents += canForm
    contents += canLbl
    contents += CommandButton("Hello"){
      println("Hallo")
    }
  }


  def top: Frame = new MainFrame {
    try {
      UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel")
    } catch {
      case e: Throwable =>
    }
    title = "TelemetryUI"
    contents = ui
    centerOnScreen()
    peer.setDefaultCloseOperation(javax.swing.WindowConstants.DO_NOTHING_ON_CLOSE)

    port = Some(SerialPortSelector())
    port.get.setBaudRate(921600)
    port.get.setNumDataBits(8)
    port.get.setNumStopBits(1)
    port.get.setParity(0)
    port.get.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 0, 0)
    if(!port.get.openPort()) println("Port could not be opened!")

    val udpServer = new UdpServer

    serialWorker = Some(new SerialWorker(port.get,
      Seq(println,canLbl.update,udpServer.broadcastCanMessage),
      Seq(println),
      Seq(() => println("Error"))
    ))
    serialWorker.get.start()
    //udpServer.start()

    val testData = new Thread{
      var running = true
      override def run(): Unit = {
        while(running){
          val msg = TelemetryMessage(1,CanFrame())
          println(msg)
          serialWorker.get.send(msg)
          Thread.sleep(1000)
        }
      }
      def close(): Unit = running = false
    }
    //testData.start()

    override def closeOperation(): Unit = {
      println("Closing")
      testData.close()
      udpServer.close()
      serialWorker.get.quit()
      port.get.closePort()
      TelemetryUI.quit()
      //exit(0)
    }

  }
}
