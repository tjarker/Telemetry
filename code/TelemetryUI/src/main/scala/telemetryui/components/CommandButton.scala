package telemetryui.components

import scala.swing.event.ButtonClicked
import scala.swing.{Button, FlowPanel}


object CommandButton {
  def apply(text: String)(callback: => Unit): Button = {
    new Button(text){
      reactions += {
        case ButtonClicked(_) => callback
      }
    }
  }
}