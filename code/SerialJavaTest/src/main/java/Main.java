
import com.fazecast.jSerialComm.*;

public class Main {
    public static void main(String[] args){
        SerialPort[] ports = SerialPort.getCommPorts();
        System.out.println(ports.length);
        for(SerialPort port : ports){
            System.out.println(port.getDescriptivePortName());
        }
    }
}
