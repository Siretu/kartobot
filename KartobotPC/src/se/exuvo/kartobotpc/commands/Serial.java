package se.exuvo.kartobotpc.commands;

import gnu.io.CommPortIdentifier;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;

import com.rapplogic.xbee.api.ApiId;
import com.rapplogic.xbee.api.AtCommand;
import com.rapplogic.xbee.api.AtCommandResponse;
import com.rapplogic.xbee.api.PacketListener;
import com.rapplogic.xbee.api.XBeeException;
import com.rapplogic.xbee.api.XBeeResponse;
import com.rapplogic.xbee.api.XBeeTimeoutException;
import com.rapplogic.xbee.api.zigbee.ZBNodeDiscover;
import com.rapplogic.xbee.util.ByteUtils;

import se.exuvo.kartobotpc.Init;
import se.exuvo.kartobotpc.Settings;
import se.exuvo.kartobotpc.comm.SerialConnection;
import se.exuvo.kartobotpc.comm.XbeeConnection;
import se.exuvo.kartobotpc.comm.ZNetNode;


public class Serial extends Command{
	
	public Serial(){
		super("serial", "Usage: [disconnect,status,connect,list] [port] [baud] [flowControl true|false]");
	}

	@Override
	public int execute(String[] tokens, String phrase) {
		SerialConnection c = Init.getSConnection();
		if(tokens.length > 1){
			if(tokens[1].equals("connect")){
				if(c.isConnected()){
					pl("Unable to connect, already connected to a port.");
					return 1;
				}
				try {
					String port = Settings.getStr("serialPort");
					int baud = Settings.getInt("serialBaud");
					boolean flow = Settings.getBol("serialFlowControl");
					
					if(tokens.length > 2){
						port = tokens[2];
					}
					if(tokens.length > 3){
						baud = new Integer(tokens[3]);
					}
					if(tokens.length > 4){
						flow = new Boolean(tokens[4]);
					}
					
					c.open(port, baud, flow);
					pl("Serial port connected.");
					return 0;
				}catch (IOException e) {
					Throwable cause = e;
					while(cause.getCause() != null){
						cause = cause.getCause();
					}
					pl("Serial port failed to connect due to: " + cause);
					return 1;
				}
				
			}else if(tokens[1].equals("disconnect")){
				if(c.isConnected()){
					c.close();
					pl("Disconnected from Serial port.");
					return 0;
				}else{
					pl("Unable to disconnect, not connected to any port.");
					return 1;
				}
				
			}else if(tokens[1].equals("status")){
				if(c.isConnected()){
					pl("Serial port connected " + c.getPort() + ":" + c.getBaud() + " FlowControl:" + c.getFlowControl());
					return 0;
				}else{
					pl("Serial port disconnected");
					return 0;
				}
				
			}else if(tokens[1].equals("list")){
				@SuppressWarnings("unchecked")
				Enumeration<CommPortIdentifier> ports = CommPortIdentifier.getPortIdentifiers();
				
				if(!ports.hasMoreElements()){
					pl("Found no serial ports.");
				}else{
					while (ports.hasMoreElements()) {
						CommPortIdentifier port = ports.nextElement();
						
						if (port.getPortType() == CommPortIdentifier.PORT_SERIAL) {
							pl("Found port: " + port.getName() + "  Owner:" + port.getCurrentOwner());
						}
					}
				}
				
				return 0;
			}
		}
		help();
		return 0;
	}
	
}
