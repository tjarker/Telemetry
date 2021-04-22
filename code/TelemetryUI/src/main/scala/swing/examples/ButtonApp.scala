package swing.examples

import scala.swing.event.ButtonClicked
import scala.swing._

object ButtonApp extends SimpleSwingApplication {
  def top: Frame = new MainFrame {
    title = "My Frame"
    contents = new GridPanel(2, 2) {
      hGap = 3
      vGap = 3
      contents += new Button {
        text = "Press Me!"
        reactions += {
          case ButtonClicked(_) => text = "Hello Scala"
        }
      }
    }
    size = new Dimension(300, 80)
  }
}
