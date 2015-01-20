package se.exuvo.kartobotpc.commands;

import java.io.IOException;
import java.util.ArrayList;
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
import se.exuvo.kartobotpc.comm.XbeeConnection;
import se.exuvo.kartobotpc.comm.ZNetNode;


public class XBee extends Command implements PacketListener{
	public static List<ZBNodeDiscover> nodes = new ArrayList<ZBNodeDiscover>();
	private long nodeDiscoveryTimeout;
	
	public XBee(){
		super("xbee", "Usage: [disconnect,status,scan,list,connect] [port] [baud] [flowControl true|false]");
	}

	@Override
	public int execute(String[] tokens, String phrase) {
		XbeeConnection c = Init.getXConnection();
		if(tokens.length > 1){
			if(tokens[1].equals("connect")){
				if(c.isConnected()){
					pl("Unable to connect, already connected to a port.");
					return 1;
				}
				try {
					String port = Settings.getStr("xbeePort");
					int baud = Settings.getInt("xbeeBaud");
					boolean flow = Settings.getBol("xbeeFlowControl");
					
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
					pl("XBee connected.");
					return 0;
				}catch (XBeeException e){
					Throwable cause = e;
					while(cause.getCause() != null){
						cause = cause.getCause();
					}
					pl("XBee failed to connect due to: " + cause);
					return 1;
				} catch (IOException e) {
					Throwable cause = e;
					while(cause.getCause() != null){
						cause = cause.getCause();
					}
					pl("XBee failed to connect due to: " + cause);
					return 1;
				}
				
			}else if(tokens[1].equals("disconnect")){
				if(c.isConnected()){
					c.close();
					pl("Disconnected from XBee.");
					return 0;
				}else{
					pl("Unable to disconnect, not connected to any port.");
					return 1;
				}
				
			}else if(tokens[1].equals("list")){
				List<ZNetNode> l = c.getNodes();
				p("Listing " + l.size() +" nodes");
				
				for(ZNetNode nd : l){
					p("\n Node: " + nd.toString());
				}
				return 0;
				
			}else if(tokens[1].equals("status")){
				if(c.isConnected()){
					pl("XBee connected " + c.getPort() + ":" + c.getBaud() + " FlowControl:" + c.getFlowControl());
					return 0;
				}else{
					pl("XBee disconnected");
					return 0;
				}
			}else if(tokens[1].equals("scan")){
				try {
					log.info("Starting scan of XBee network");
					// get the Node discovery timeout
					nodeDiscoveryTimeout=0;
					c.addPacketListener(this);
					c.send(new AtCommand("NT"));
					
					long start = System.currentTimeMillis();
					while(nodeDiscoveryTimeout == 0){
						try {
							Thread.sleep(1);
						} catch (InterruptedException e) {
						}
						if(System.currentTimeMillis() - start > 100){
							XBeeTimeoutException x = new XBeeTimeoutException();
							throw x;
						}
					}
					
					nodes.clear();
					log.debug("Sending node discover command");
					c.send(new AtCommand("ND"));
					
					try {
						Thread.sleep(nodeDiscoveryTimeout);
					} catch (InterruptedException e) {
					}
					
					String message = "Found " + nodes.size() + " nodes:";
					for(Iterator<ZBNodeDiscover> it=nodes.iterator();it.hasNext();){
						ZBNodeDiscover n = it.next();
						log.info("Node discover response is: " + n);
						message += "\n Node: " + n.toString();
					}
					
					pl(message);
					return 0;
				} catch (XBeeTimeoutException e) {
					log.error("XBee scan error: ", e);
					pl("XBee scan failed: " + e.getMessage());
					return 1;
				} catch (XBeeException e) {
					log.error("XBee scan error: ", e);
					pl("XBee scan failed:" + e.getMessage());
					return 1;
				}finally{
					c.removePacketListener(this);
				}
			}
		}
		help();
		return 0;
	}
	
	@Override
	public void processResponse(XBeeResponse response) {
		if(response.getApiId() == ApiId.AT_RESPONSE){
			AtCommandResponse atResponse = (AtCommandResponse)response;
			if(atResponse.getCommand().equals("ND")){
				ZBNodeDiscover nd = ZBNodeDiscover.parse((AtCommandResponse)response);
				nodes.add(nd);
			}else if(atResponse.getCommand().equals("NT")){
				// default is 6 seconds
				nodeDiscoveryTimeout = ByteUtils.convertMultiByteToInt(atResponse.getValue()) * 100;
				log.info("Node discovery timeout is " + nodeDiscoveryTimeout + " milliseconds");
			}
			
		}
	}

}
