package telemetryui.serial

import telemetryui.types.{CanFrame, TelemetryMessage}

class JsonAssembler {

  private val charBuf = new StringBuilder
  private var openBrackets = 0
  private var telemetryMessage: TelemetryMessage = null
  private var error = false
  private var hasNewFrame = false


  def add(byte: Byte): Unit = {

    val c = byte.toChar

    if (c == '\n') return

    charBuf.append(c)

    c match {
      case '{' => openBracket()
      case '}' => closeBracket()
      case _ => if (charBuf.size == 1) charBuf.clear()
    }
  }

  private def openBracket(): Unit = openBrackets += 1

  private def closeBracket(): Unit = {
    if (openBrackets == 0) {

      charBuf.clear()

    } else if (openBrackets == 1) {

      openBrackets = 0

      try {
        telemetryMessage = TelemetryMessage(charBuf.toString)
        hasNewFrame = true
      } catch {
        case e: org.json4s.MappingException =>
          error = true
      }

      charBuf.clear()

    } else {
      openBrackets -= 1
    }
  }

  def hasMessage: Boolean = hasNewFrame

  def getMessage: TelemetryMessage = {
    hasNewFrame = false
    return telemetryMessage
  }

  def hadError: Boolean = {
    val tmp = error
    error = false
    return tmp
  }

}
