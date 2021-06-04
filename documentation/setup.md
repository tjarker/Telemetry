<a name="top"></a>
[back to README](../README.md)


# Telemetry UI

The TelemetryUI needs full access to the serial interface of the Teensy. The following command fetches relevant udev rules and places them in the correct folder:

```shell
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/master/scripts/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
```