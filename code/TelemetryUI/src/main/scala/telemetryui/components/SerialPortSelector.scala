package telemetryui.components

import com.fazecast.jSerialComm.{SerialPort, SerialPortInvalidPortException}
import telemetryui.serial.SerialPortListener
import telemetryui.types.CanFrame

import scala.swing.Dialog.{Message, showInput}
import scala.swing.Swing

object SerialPortSelector {
  def apply(listener: (CanFrame => Unit)*): SerialPort = {
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
      val port = SerialPort.getCommPort(ports(possibilities.indexOf(s.get)).getSystemPortName)
      port.setBaudRate(115200)
      port.setNumDataBits(8)
      port.setNumStopBits(1)
      port.setParity(0)
      port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0)
      port.openPort()

      port.addDataListener(new SerialPortListener(port, listener))
      return port
    }
    else
      throw new SerialPortInvalidPortException()
  }
}