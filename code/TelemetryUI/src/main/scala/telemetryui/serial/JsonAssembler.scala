package telemetryui.serial

import telemetryui.types.CanFrame

class JsonAssembler[T <: Any] {

  private val charBuf = new StringBuilder
  private var openBrackets = 0
  private var canFrame: CanFrame = null
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
        canFrame = CanFrame(charBuf.toString)
        hasNewFrame = true
      } catch {
        case e: org.json4s.MappingException => error = true
      }

      charBuf.clear()

    } else {
      openBrackets -= 1
    }
  }

  def hasFrame: Boolean = hasNewFrame

  def getFrame: CanFrame = {
    hasNewFrame = false
    return canFrame
  }

  def hadError: Boolean = {
    val tmp = error
    error = false
    return tmp
  }

}
