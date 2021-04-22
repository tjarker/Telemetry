package swing.examples

import scala.swing.event.ButtonClicked
import scala.swing._

object SwingApp extends SimpleSwingApplication {
  def top: Frame = new MainFrame {
    title = "SwingApp"
    var numClicks = 0

    object label extends Label {
      val prefix = "Number of button clicks: "
      text = prefix + "0  "
      listenTo(button)
      reactions += {
        case ButtonClicked(_) =>
          numClicks = numClicks + 1
          text = prefix + numClicks
      }
    }

    object button extends Button {
      text = "I am a button"
    }

    contents = new FlowPanel {
      contents ++= Seq(button, label)
      border = Swing.EmptyBorder(5, 5, 5, 5)
    }
  }
}
