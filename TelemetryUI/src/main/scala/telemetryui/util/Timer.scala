package telemetryui.util

object Timer {
  def apply(interval: Int, repeats: Boolean = true)(op: => Unit): javax.swing.Timer = {
    val timeOut = new javax.swing.AbstractAction() {
      def actionPerformed(e: java.awt.event.ActionEvent) = op
    }
    val t = new javax.swing.Timer(interval, timeOut)
    t.setRepeats(repeats)
    t.start()
    t
  }
}
