package swing.examples

import scala.swing.event.ButtonClicked
import scala.swing._

object CountButton extends SimpleSwingApplication {
  def top: Frame = new MainFrame {
    title = "My Frame"
    contents = new GridPanel(2, 2) {
      hGap = 3
      vGap = 3
      val button = new Button {
        text = "Press Me!"
      }
      contents += button
      val label = new Label {
        text = "No button clicks registered"
      }
      contents += label

      listenTo(button)
      var nClicks = 0
      reactions += {
        case ButtonClicked(_) =>
          nClicks += 1
          label.text = "Number of button clicks: " + nClicks
      }
    }
  }
}
