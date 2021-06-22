package telemetryui.components

import scala.swing.event.ButtonClicked
import scala.swing.{Button, FlowPanel, ToggleButton}

/**
 * A button which calls the passed function on click
 */
object CommandButton {
  def apply(text: String)(callback: => Unit): Button = {
    new Button(text){
      reactions += {
        case ButtonClicked(_) => callback
      }
    }
  }
}


/**
 * A toggle button which calls the passed functions on select and unselect
 */
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