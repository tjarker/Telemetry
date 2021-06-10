package telemetryui.components

import telemetryui.types.{CanFrame, TimeStamp}

import scala.swing.{Alignment, Button, GridBagPanel, Label, TextField}
import scala.swing.Swing.{CompoundBorder, EmptyBorder, EtchedBorder, TitledBorder}
import scala.swing.event.{ButtonClicked, FocusGained, KeyTyped}
import scala.swing.event._
import telemetryui.util.Timer

import java.util.Date
import javax.swing.UIManager

class CanFrameForm(title: String, sender: CanFrame => Unit) extends GridBagPanel {
  border = CompoundBorder(TitledBorder(EtchedBorder,title),EmptyBorder(5,5,5,5))

  try {
    UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel")
  } catch {
    case e: Throwable =>
  }

  val c = new Constraints()

  c.fill = GridBagPanel.Fill.Horizontal
  c.ipadx = 5

  c.grid = (0,0)
  layout(new Label("id")) = c

  c.grid = (1,0)
  layout(new Label("data")) = c

  c.grid = (0,1)
  val id = new TextField{
    text = "0"
    columns = 4
    horizontalAlignment = Alignment.Right
    listenTo(keys)
    reactions += {
      case e: KeyTyped => {
        if(text.length <= 1){
          if(!(e.char == 'x' && text == "0") && !e.char.isDigit) e.consume()
        } else {
          if(text.startsWith("0x")){
            if(Character.digit(e.char,16) == -1) e.consume()
          } else {
            if(!e.char.isDigit) e.consume()
          }
        }
      }
    }
    def value: Int = if(isHex) java.lang.Integer.valueOf(text.substring(2),16) else java.lang.Integer.valueOf(text,10)
    def isHex: Boolean = text.startsWith("0x")
  }
  layout(id) = c

  c.grid = (1,1)
  val msg = new TextField{
    text = "0"
    columns = 20
    horizontalAlignment = Alignment.Right
    listenTo(keys)
    reactions += {
      case e: FocusGained => if(text == "0") text = ""
      case e: KeyTyped => {
        if(text.length <= 1){
          if(!(e.char == 'x' && text == "0") && !e.char.isDigit) e.consume()
        } else {
          if(isHex){
            if(Character.digit(e.char,16) == -1) e.consume()
          } else {
            if(!e.char.isDigit) e.consume()
          }
        }
      }
    }
    def value: BigInt = if(isHex) BigInt(text.substring(2),16) else BigInt(text,10)
    def isHex: Boolean = text.startsWith("0x")
    def numBytes: Int = (Math.ceil((Math.log10(value.toLong) / Math.log10(2))/8)).toInt
  }
  layout(msg) = c

  c.anchor = GridBagPanel.Anchor.East
  c.ipadx = 20
  c.grid = (2,1)
  val sendBtn = new Button {
    text = "send"
  }
  layout(sendBtn) = c

  c.gridwidth = 3
  c.grid = (0,3)
  val error = new Label {
    foreground = java.awt.Color.red
    text = ""
    def setError(src: String): Unit = text = s"$src out of range!"
    def clearError(): Unit = text = ""
  }
  layout(error) = c


  listenTo(sendBtn,keys)
  reactions += {
    case ButtonClicked(`sendBtn`) =>
      val idVal = id.value
      val dataVal = msg.value
      if(idVal > Math.pow(2,12)-1) error.setError("id")
      else if(dataVal.toLong > Math.pow(2,64)) error.setError("data")
      else{
        sender(CanFrame(idVal,false,msg.numBytes,dataVal,TimeStamp()))
      }
      Timer(4000,false){error.clearError()}
  }
}

