package telemetryui.components

import telemetryui.types.CanFrame

import scala.swing.Swing.{CompoundBorder, EmptyBorder, EtchedBorder, TitledBorder}
import scala.swing._


class CanFrameLabel(title: String) extends GridBagPanel {
  border = CompoundBorder(TitledBorder(EtchedBorder,title),EmptyBorder(5,5,5,5))

  val c = new Constraints()

  c.fill = GridBagPanel.Fill.Horizontal


  c.grid = (0,0)
  c.weightx = 0.3
  layout(new Label("id")) = c

  c.grid = (1,0)
  c.weightx = 0.2
  layout(new Label("len")) = c

  c.grid = (2,0)
  c.weightx = 0.5
  layout(new Label("data")) = c

  c.grid = (0,1)
  val id = new Label{
    text = "%04d".format(0)
    horizontalAlignment = Alignment.Center
  }
  layout(id) = c

  c.grid = (1,1)
  val len = new Label{
    text = "%01d".format(0)
    horizontalAlignment = Alignment.Center
  }
  layout(len) = c

  c.grid = (2,1)
  val msg = new Label{
    text = "%019d".format(0)
    horizontalAlignment = Alignment.Center
  }
  layout(msg) = c

  def update(canFrame: CanFrame): Unit = {
    id.text = canFrame.id.toString(16)
    len.text = canFrame.len.toHexString
    msg.text = canFrame.data.toString(16)
  }

}
