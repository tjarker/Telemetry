package swing.examples


import scala.swing.event._
import scala.swing._
import java.awt.{Color,geom}
/**
 * Dragging the mouse draws a simple graph
 *
 * @author Frank Teubler, Ingo Maier
 */
object LinePainting extends SimpleSwingApplication {
  lazy val ui: Panel = new Panel {
    background = Color.white
    preferredSize = new Dimension(300, 200)

    focusable = true
    listenTo(mouse.clicks, mouse.moves, keys)

    reactions += {
      case e: MousePressed =>
        moveTo(e.point)
        requestFocusInWindow()
      case e: MouseDragged => lineTo(e.point)
      case e: MouseReleased => lineTo(e.point)
      case KeyTyped(_, 'c', _, _) =>
        path = new geom.GeneralPath
        repaint()
      case _: FocusLost => repaint()
    }

    /* records the dragging */
    var path = new geom.GeneralPath

    def lineTo(p: Point): Unit = {
      path.lineTo(p.x.toFloat, p.y.toFloat);
      repaint()
    }

    def moveTo(p: Point): Unit = {
      path.moveTo(p.x.toFloat, p.y.toFloat);
      repaint()
    }

    override def paintComponent(g: Graphics2D): Unit = {
      super.paintComponent(g)
      g.setColor(new Color(100, 100, 100))
      val h = size.height
      g.drawString("Press left mouse button and drag to paint.", 10, h - 26)
      if (hasFocus) g.drawString("Press 'c' to clear.", 10, h - 10)
      g.setColor(Color.black)
      g.draw(path)
    }
  }

  def top: Frame = new MainFrame {
    title = "Simple Line Painting Demo"
    contents = ui
  }
}
