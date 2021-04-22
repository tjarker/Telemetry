package swing.examples

import scala.swing.event.MousePressed
import scala.swing.{Frame, Label, MainFrame, SimpleSwingApplication}

object LabelTest extends SimpleSwingApplication {
  def top: Frame = new MainFrame {
    contents = new Label {
      text = "Hello"
      listenTo(mouse.clicks)
      reactions += {
        case MousePressed(_, _, _, _, _) =>
          println("Mouse pressed")
      }
    }
  }
}
