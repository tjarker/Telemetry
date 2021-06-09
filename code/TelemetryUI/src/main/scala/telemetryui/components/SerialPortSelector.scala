package telemetryui.components

import com.fazecast.jSerialComm.{SerialPort, SerialPortInvalidPortException}
import telemetryui.serial.SerialPortListener
import telemetryui.types.CanFrame

import scala.swing.Dialog.{Message, showInput}
import scala.swing.Swing

object SerialPortSelector {
  def apply(): SerialPort = {
    val ports = SerialPort.getCommPorts
    val possibilities = ports.map(p => p.getSystemPortName + " - " + p.getDescriptivePortName)
    val s = showInput(null,
      "Select the serial port to connect to:",
      "Serial Port",
      Message.Plain,
      Swing.EmptyIcon,
      possibilities,
      possibilities.head
    )

    if (s.isDefined && s.get.nonEmpty) {
      println(s"Selected Port has system port name: ${ports(possibilities.indexOf(s.get)).getSystemPortName}")
      return SerialPort.getCommPort(ports(possibilities.indexOf(s.get)).getSystemPortName)
    }
    else
      throw new SerialPortInvalidPortException()
  }
}
