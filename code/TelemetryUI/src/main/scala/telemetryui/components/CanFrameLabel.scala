package telemetryui.components

import telemetryui.types.CanFrame

import scala.swing.GridBagPanel.Anchor
import scala.swing.Swing.{CompoundBorder, EmptyBorder, EtchedBorder, TitledBorder}
import scala.swing._
import scala.swing.event.{ButtonClicked, SelectionChanged}


class CanFrameLabel(title: String) extends GridBagPanel {
  border = CompoundBorder(TitledBorder(EtchedBorder,title),EmptyBorder(5,5,5,5))

  private var lastCanMsg: CanFrame = null

  val c = new Constraints()

  c.fill = GridBagPanel.Fill.Horizontal

  c.grid = (0,0)
  c.weightx = 0.2
  layout(new Label("time")) = c

  c.grid = (1,0)
  c.weightx = 0.3
  layout(new Label("id")) = c

  c.grid = (2,0)
  c.weightx = 0.2
  layout(new Label("len")) = c

  c.grid = (3,0)
  c.weightx = 0.5
  layout(new Label("data")) = c

  c.grid = (0,1)
  val time = new Label{
    text = "00:00:00"
    horizontalAlignment = Alignment.Center
  }
  layout(time) = c

  c.grid = (1,1)
  val id = new Label{
    text = "%04d".format(0)
    horizontalAlignment = Alignment.Center
  }
  layout(id) = c

  c.grid = (2,1)
  val len = new Label{
    text = "%01d".format(0)
    horizontalAlignment = Alignment.Center
  }
  layout(len) = c

  c.grid = (3,1)
  val msg = new Label{
    text = "%019d".format(0)
    horizontalAlignment = Alignment.Center
  }
  layout(msg) = c

  c.anchor = Anchor.East
  c.gridheight = 2
  c.grid = (4,0)
  val hexChecker = new CheckBox{
    text = "hex"
    reactions += {
      case e: scala.swing.event.ActionEvent => update(lastCanMsg)
    }
  }
  layout(hexChecker) = c

  def update(canFrame: CanFrame): Unit = {
    lastCanMsg = canFrame
    len.text = canFrame.len.toString
    time.text = canFrame.stamp.toString
    if(hexChecker.selected){
      id.text = canFrame.id.toString(16)
      msg.text = canFrame.data.toString(16)
    } else {
      id.text = canFrame.id.toString(10)
      msg.text = canFrame.data.toString(10)
    }


  }

}
