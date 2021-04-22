package swing.examples

import scala.swing.{Button, Frame, MainFrame, SimpleSwingApplication}

/**
 * A simple swing demo.
 */
object HelloWorld extends SimpleSwingApplication {
  def top: Frame = new MainFrame {
    title = "Hello, World!"
    contents = new Button("Click Me!")
  }
}
