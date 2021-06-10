package telemetryui

import com.fazecast.jSerialComm.SerialPort
import org.json4s.DefaultFormats
import org.json4s.jackson.Serialization
import telemetryui.components.{CanFrameForm, CanFrameLabel, CommandButton, SerialPortSelector, ToggleCommandButton}
import telemetryui.serial.SerialWorker
import telemetryui.types.CMD.{BROADCAST_CAN, CONNECT_CAN, DISCONNECT_CAN, START_LOGGING, START_STREAMING, STOP_LOGGING, STOP_STREAMING}
import telemetryui.types.{CanFrame, TelemetryMessage}
import telemetryui.udp.UdpServer
import telemetryui.util.Timer

import java.awt.Dimension
import javax.swing.UIManager
import scala.swing.Swing.{CompoundBorder, EmptyBorder, EtchedBorder, TitledBorder}
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
    serialWorker.get.send(TelemetryMessage(BROADCAST_CAN, Some(canFrame)))
  })

  lazy val ui = new BoxPanel(Orientation.Vertical) {
    border = EmptyBorder(10, 10, 10, 10)
    contents += canForm
    contents += canLbl
    contents += new BoxPanel(Orientation.Horizontal){
      border = CompoundBorder(TitledBorder(EtchedBorder,"Commands"),EmptyBorder(5,5,5,5))

      val loggingBtn = ToggleCommandButton("Logging"){
        println(s"Start Logging")
        serialWorker.get.send(TelemetryMessage(START_LOGGING, None))
      }{
        println(s"Stop Logging")
        serialWorker.get.send(TelemetryMessage(STOP_LOGGING, None))
      }
      val streamingBtn = ToggleCommandButton("Streaming"){
        println(s"Start Streaming")
        serialWorker.get.send(TelemetryMessage(START_STREAMING, None))
      }{
        println(s"Stop Streaming")
        serialWorker.get.send(TelemetryMessage(STOP_STREAMING, None))
      }
      val sleepBtn = ToggleCommandButton("Sleep"){
        println("Putting to sleep")
        if(loggingBtn.selected) loggingBtn.doClick()
        loggingBtn.enabled = false
        if(streamingBtn.selected) streamingBtn.doClick()
        streamingBtn.enabled = false
        serialWorker.get.send(TelemetryMessage(DISCONNECT_CAN, None))
      }{
        println("Waking up")
        serialWorker.get.send(TelemetryMessage(CONNECT_CAN, None))
        loggingBtn.enabled = true
        streamingBtn.enabled = true
      }
      contents ++= Seq(sleepBtn,streamingBtn,loggingBtn)
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
          val msg = TelemetryMessage(1,Some(CanFrame()))
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
