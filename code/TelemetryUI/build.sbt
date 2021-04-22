name := "TelemetryUI"

version := "0.1"

scalaVersion := "2.11.12"



libraryDependencies += "org.scala-lang.modules" %% "scala-swing" % "3.0.0"

libraryDependencies += "com.fazecast" % "jSerialComm" % "[2.0.0,3.0.0)"

libraryDependencies += "org.json4s" %% "json4s-jackson" % "3.2.11"

mainClass in (Compile, run) := Some("telemetryui.TelemetryUI")

mainClass in (Compile, packageBin) := Some("telemetryui.TelemetryUI")