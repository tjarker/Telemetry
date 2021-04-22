package trash

import javax.swing.UIManager
import scala.swing.Swing.{CompoundBorder, EmptyBorder, EtchedBorder, TitledBorder}
import scala.swing._

object CanFrameLabelOld extends SimpleSwingApplication {

  val id = new Label("   0") {
    horizontalAlignment = Alignment.Center
  }
  val dlc = new Label("0") {
    horizontalAlignment = Alignment.Center
  }
  val msg = new Label("                   0") {
    horizontalAlignment = Alignment.Center
  }

  lazy val ui = new GridPanel(2, 3) {
    border = CompoundBorder(TitledBorder(EtchedBorder, "Received CAN Message"), EmptyBorder(5, 5, 5, 5))
    contents ++= Seq(
      new Label("ID") {
        horizontalAlignment = Alignment.Center
      },
      new Label("DLC") {
        horizontalAlignment = Alignment.Center
      },
      new Label("Message") {
        horizontalAlignment = Alignment.Center
      },
      id,
      dlc,
      msg
    )


  }

  def top: Frame = new MainFrame {
    title = "CAN Frame Entry"
    try {
      UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel")
    } catch {
      case e: Throwable =>
    }
    contents = ui
  }
}
