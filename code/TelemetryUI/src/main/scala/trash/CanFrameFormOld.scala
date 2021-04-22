package trash

import javax.swing.UIManager
import scala.swing.Swing.{CompoundBorder, EmptyBorder, EtchedBorder, TitledBorder}
import scala.swing.event.KeyTyped
import scala.swing._

object CanFrameFormOld extends SimpleSwingApplication {


  val id = new TextField {
    text = ""
    columns = 10
    horizontalAlignment = Alignment.Center
    listenTo(keys)
    reactions += {
      case e: KeyTyped =>
        if (!e.char.isDigit) e.consume()
    }
  }
  val rtr = new CheckBox("") {
    horizontalAlignment = Alignment.Center
    border = EmptyBorder(0, 5, 0, 5)
  }
  val msg = new TextField {
    text = ""
    columns = 10
    horizontalAlignment = Alignment.Center

  }
  listenTo(id, msg)
  reactions += {
    case e: KeyTyped => if (!e.char.isDigit) e.consume()
  }

  lazy val ui = new GridPanel(2, 3) {
    border = CompoundBorder(TitledBorder(EtchedBorder, "CAN Message"), EmptyBorder(5, 5, 5, 5))
    contents ++= Seq(
      new Label("ID") {
        horizontalAlignment = Alignment.Center
      },
      new Label("RTR") {
        horizontalAlignment = Alignment.Center
      },
      new Label("Message") {
        horizontalAlignment = Alignment.Center
      },
      id,
      rtr,
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
