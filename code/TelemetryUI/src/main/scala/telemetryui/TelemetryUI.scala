package telemetryui

import com.fazecast.jSerialComm.SerialPort
import org.json4s.DefaultFormats
import org.json4s.jackson.Serialization
import telemetryui.components.{CanFrameForm, CanFrameLabel, SerialPortSelector}
import telemetryui.serial.SerialWorker
import telemetryui.types.CMD.BROADCAST_CAN
import telemetryui.types.TelemetryMessage

import java.awt.Dimension
import javax.swing.UIManager
import scala.swing.Swing.EmptyBorder
import scala.swing._
import scala.sys.exit

object TelemetryUI extends SimpleSwingApplication {
  implicit val formats = DefaultFormats

  var port: SerialPort = null

  val canLbl = new CanFrameLabel("Last Received CAN Frame")
  val canForm = new CanFrameForm("Send CAN Frame", { canFrame =>
    val outBytes = TelemetryMessage(BROADCAST_CAN, canFrame).toByteArray
    println(outBytes.map(_.toString).mkString(", "))
    port.writeBytes(outBytes, outBytes.length)
  })

  lazy val ui = new BoxPanel(Orientation.Vertical) {
    border = EmptyBorder(10, 10, 10, 10)
    contents += canForm
    contents += canLbl
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

    port = SerialPortSelector()
    port.setBaudRate(921600)
    port.setNumDataBits(8)
    port.setNumStopBits(1)
    port.setParity(0)
    port.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 0, 0)
    port.openPort()

    val serialWorker = new SerialWorker(port,Seq(println,canLbl.update),Seq(println),Seq(() => println("Error")))
    serialWorker.start()

    override def closeOperation(): Unit = {
      println("Closing")
      serialWorker.quit()
      exit(0)
    }

  }
}
