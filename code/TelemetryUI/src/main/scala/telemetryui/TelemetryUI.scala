package telemetryui

import com.fazecast.jSerialComm.SerialPort
import org.json4s.DefaultFormats
import org.json4s.jackson.Serialization
import telemetryui.TelemetryUI.port
import telemetryui.components.{CanFrameForm, CanFrameLabel, CommandButton, SerialPortSelector, ToggleCommandButton}
import telemetryui.serial.SerialWorker
import telemetryui.types.CMD._
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
      var oldStateLoggingButton = false
      var oldStateStreamingButton = false
      val sleepBtn = ToggleCommandButton("Sleep"){
        println("Putting to sleep")
        oldStateLoggingButton = loggingBtn.selected
        if(loggingBtn.selected) {
          loggingBtn.selected = false
        }
        loggingBtn.enabled = false
        oldStateStreamingButton = streamingBtn.selected
        if(streamingBtn.selected){
          streamingBtn.selected = false
        }
        streamingBtn.enabled = false
        serialWorker.get.send(TelemetryMessage(SLEEP, None))
      }{
        println("Waking up")
        serialWorker.get.send(TelemetryMessage(WAKE_UP, None))
        loggingBtn.enabled = true
        streamingBtn.enabled = true
        loggingBtn.selected = oldStateLoggingButton
        streamingBtn.selected = oldStateStreamingButton
      }
      val encryptionBtn = ToggleCommandButton("Encryption"){
        serialWorker.get.send(TelemetryMessage(ENABLE_ENCRYPTION, None))
        println("Enable encryption")
      }{
        serialWorker.get.send(TelemetryMessage(DISABLE_ENCRYPTION, None))
        println("Disable encyrption")
      }
      contents ++= Seq(sleepBtn,streamingBtn,loggingBtn,encryptionBtn)
    }
  }


  def top: Frame = new MainFrame {
    try {
      UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel")
    } catch {
      case e: Throwable => println("Could not set GTK look and feel")
    }
    title = "TelemetryUI"
    contents = ui
    centerOnScreen()
    peer.setDefaultCloseOperation(javax.swing.WindowConstants.DO_NOTHING_ON_CLOSE)

    port = SerialPortSelector()
    port match {
      case Some(port) => {
        port.setBaudRate(921600)
        port.setNumDataBits(8)
        port.setNumStopBits(1)
        port.setParity(0)
        port.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 0, 0)
        if(!port.openPort()) println("Port could not be opened!")
      }
      case None => {
        TelemetryUI.quit()
      }
    }


    val udpServer = new UdpServer

    serialWorker = Some(new SerialWorker(port.get,
      Seq(println,canLbl.update,udpServer.broadcastCanMessage),
      Seq(println),
      Seq(() => println("Error"))
    ))
    serialWorker.get.start()
    udpServer.start()

    override def closeOperation(): Unit = {
      println("Closing")
      udpServer.close()
      serialWorker.get.quit()
      port.get.closePort()
      TelemetryUI.quit()
      //exit(0)
    }

  }
}
