package se.exuvo.kartobotpc.commands;

import java.io.IOException;
import java.util.Arrays;

import se.exuvo.kartobotpc.Init;
import se.exuvo.kartobotpc.comm.Devices;
import se.exuvo.kartobotpc.gui.GUI;
import se.exuvo.kartobotpc.gui.Main;

import com.rapplogic.xbee.api.XBeeException;
import com.rapplogic.xbee.api.XBeeTimeoutException;
import com.rapplogic.xbee.util.ByteUtils;


public class Send extends Command {

	public Send(){
		super("send", "Sends data to Quad. usage: send (0x)####..");
	}
	
	@Override
	public int execute(String[] tokens, String phrase) {
		int[] d = new int[phrase.length() - tokens[0].length() + 1];
		int p=0;
		
		if(tokens.length > 1 && tokens[1].charAt(0) == 'c'){//Console input
			String s = phrase.substring(tokens[0].length() + 1);
			for(int a=0; a < s.length(); a++){
				d[p++] = s.charAt(a);
			}
			d[p++] = '\r';
			((Main)Init.getGUI().getState(GUI.MAINSTATE)).console2Append(s + "\n");
		}else{
			for(int i=1; i < tokens.length && p < d.length; i++){
				String s = tokens[i].trim();
				
				if(s.matches("[^0-9]+")){
					for(int a=0; a < s.length(); a++){
						d[p++] = s.charAt(a);
					}
				}else if(s.matches("-?(0x)?[0-9a-fA-F]{1,3}")){
					if(s.contains("-")){
						d[p++] = 0x80 | Integer.decode(s.replace("-", ""));
					}else{
						d[p++] = Integer.decode(s);
					}
					
				}else{
					pl("Unknow how to handle \"" + s + "\"");
					return 2;
				}
			}
		}
		
		d = Arrays.copyOf(d, p);
		
		if(Init.getSConnection().isConnected()){
			try {
				synchronized(Devices.getDevices()){
					Init.getSConnection().send(d);
				}
			} catch (IOException e) {
				log.warn("", e);
				e.printStackTrace(getOut());
				pl("Failed");
				return 1;
			}
			pl("Sent " + d.length + " bytes via USB: " + ByteUtils.toBase16(d));
			return 0;
		}else if(Init.getXConnection().isConnected()){
			try {
				synchronized(Devices.getDevices()){
					Init.getXConnection().send(Devices.getDevices().get(0).getAddress(),d, true);
				}
			} catch (XBeeTimeoutException e) {
				log.warn("", e);
				e.printStackTrace(getOut());
				pl("Failed");
				return 1;
			} catch (XBeeException e) {
				log.warn("", e);
				e.printStackTrace(getOut());
				pl("Failed");
				return 1;
			}
			pl("Sent " + d.length + " bytes: " + ByteUtils.toBase16(d));
			return 0;
		}else{
			pl("XBee is disconnected");
			return 1;
		}
	}

}
