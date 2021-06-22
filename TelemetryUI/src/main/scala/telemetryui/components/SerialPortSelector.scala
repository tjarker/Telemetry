package telemetryui.components

import com.fazecast.jSerialComm.{SerialPort, SerialPortInvalidPortException}
import telemetryui.serial.SerialPortListener
import telemetryui.types.CanFrame

import scala.swing.Dialog.{Message, showInput}
import scala.swing.Swing

/**
 * A pop up window to choose a serial port
 */

object SerialPortSelector {
  def apply(): Option[SerialPort] = {
    val ports = SerialPort.getCommPorts
    val possibilities = ports.map(p => p.getSystemPortName + " - " + p.getDescriptivePortName)
    val s = showInput(null,
      "Select the serial port to connect to:",
      "Serial Port",
      Message.Plain,
      Swing.EmptyIcon,
      possibilities,
      if (!possibilities.isEmpty) possibilities.head else "No ports found"
    )

    s match {
      case Some("No ports found") => {
        this.apply()
      }
      case Some(str) => {
        println(s"Selected Port has system port name: ${ports(possibilities.indexOf(s.get)).getSystemPortName}")
        return Some(SerialPort.getCommPort(ports(possibilities.indexOf(str)).getSystemPortName))
      }
      case None => {
        return None
      }
    }
  }
}
