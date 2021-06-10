package telemetryui.components

import scala.swing.event.ButtonClicked
import scala.swing.{Button, FlowPanel, ToggleButton}


object CommandButton {
  def apply(text: String)(callback: => Unit): Button = {
    new Button(text){
      reactions += {
        case ButtonClicked(_) => callback
      }
    }
  }
}

object ToggleCommandButton {
  def apply(text: String)(selectCallback: => Unit)(unselectCallBack: => Unit): ToggleButton = {
    new ToggleButton(text){
      reactions += {
        case ButtonClicked(_) => {
          if(this.selected) selectCallback
          else unselectCallBack
        }
      }
    }
  }
}