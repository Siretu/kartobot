package se.exuvo.kartobotpc.comm;

import gnu.io.SerialPort;

import java.io.IOException;
import java.security.InvalidParameterException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

import org.apache.log4j.Logger;

import se.exuvo.kartobotpc.Init;
import se.exuvo.kartobotpc.Settings;

import com.rapplogic.xbee.RxTxSerialComm;
import com.rapplogic.xbee.api.AtCommand;
import com.rapplogic.xbee.api.AtCommandResponse;
import com.rapplogic.xbee.api.ErrorResponse;
import com.rapplogic.xbee.api.ModemStatusResponse;
import com.rapplogic.xbee.api.PacketListener;
import com.rapplogic.xbee.api.XBee;
import com.rapplogic.xbee.api.XBeeAddress16;
import com.rapplogic.xbee.api.XBeeAddress64;
import com.rapplogic.xbee.api.XBeeException;
import com.rapplogic.xbee.api.XBeeResponse;
import com.rapplogic.xbee.api.XBeeTimeoutException;
import com.rapplogic.xbee.api.zigbee.ZBNodeDiscover;
import com.rapplogic.xbee.api.zigbee.ZNetNodeIdentificationResponse;
import com.rapplogic.xbee.api.zigbee.ZNetRxResponse;
import com.rapplogic.xbee.api.zigbee.ZNetTxRequest;
import com.rapplogic.xbee.api.zigbee.ZNetTxStatusResponse;

import de.matthiasmann.twl.utils.CallbackSupport;


/*
 * Input med event listener
 * Packet format = id,length,data,checksum
 * 
 */
public class XbeeConnection implements PacketListener {
	private static final Logger log = Logger.getLogger(XbeeConnection.class);
	private int lastRSSI = Integer.MIN_VALUE;
	private long RSSIcooldown = System.currentTimeMillis();
	private XBee xbee = new XBee();
	public static final int maxPacketSize = 90;//72 data + packet header (5) + frame header(max 13)
	public static final int maxPacketDataSize = 72;
	public long sendError = 0;
	public long receiveError = 0;
	public long lostTCPPackets = 0;
	public AtomicInteger syncWaiting = new AtomicInteger();
	private final Hashtable<String, XBeeAddress16> addresses = new Hashtable<String, XBeeAddress16>();
	private final List<ZNetNode> nodes = new ArrayList<ZNetNode>();
	private final List<TCP> tcpList = new ArrayList<TCP>();
	private final List<Sent> sent = new ArrayList<Sent>();
	public AtomicInteger tcpID = new AtomicInteger();
	private ScheduledExecutorService executor = Executors.newSingleThreadScheduledExecutor();
	private String port = "";
	private int baud = 0;
	private boolean flowControl = false;
	public int speedIn = 0;
	public int speedOut = 0;
	private int speedIn2 = 0;
	private int speedOut2 = 0;
	
	private Runnable[] connectCallbacks;

    public void addConnectCallback(Runnable callback) {
    	connectCallbacks = CallbackSupport.addCallbackToList(connectCallbacks, callback, Runnable.class);
    }

    public void removeConnectCallback(Runnable callback) {
    	connectCallbacks = CallbackSupport.removeCallbackFromList(connectCallbacks, callback);
    }

    protected void doConnectCallback() {
        CallbackSupport.fireCallbacks(connectCallbacks);
    }
	
	public class TCP {
		public ZNetTxRequest data;
		public long sent;
		public int id;
		public int retries;
		
		public TCP(ZNetTxRequest d){
			data = d;
			sent = System.currentTimeMillis();
			id = data.getPayload()[1];
		}
	}
	
	public class Sent {
		public XBeeAddress16 a16;
		public XBeeAddress64 a64;
		public int frameId;
		
		public Sent(int frameId, XBeeAddress64 a64, XBeeAddress16 a16){
			this.frameId = frameId;
			this.a64 = a64;
			this.a16 = a16;
		}
	}
	
	public XbeeConnection(){
		if(Settings.getBol("logSensors")){
			Init.addSensor("remote error", "", "type");
			Init.addSensor("battery U", "U", "mV");
		}
		new Thread(){
			public void run(){
				while(true){
					synchronized (tcpList) {
						if(isConnected()){
							for(Iterator<TCP> it = tcpList.iterator();it.hasNext();){
								TCP t = it.next();
								if(System.currentTimeMillis() - t.sent > 100){
									if(t.retries < 2){
										try {
											log.debug("Resending(" + (t.retries+1) + ") TCP: " + t.id);
											send(t.data, false);
										} catch (XBeeException e) {
											e.printStackTrace();
										} finally{
											t.sent = System.currentTimeMillis();
											t.retries++;
											lostTCPPackets++;
										}
									}else{
										it.remove();
									}
								}
							}
						}else{
							tcpList.clear();
						}
					}
					try{
			    	    Thread.sleep(50);
			    	}catch(InterruptedException e){
			    	}
				}
			}
		}.start();
		
		new Thread(){
			public void run(){
				while(true){
					speedIn = speedIn2;
					speedIn2 = 0;
					speedOut = speedOut2;
					speedOut2 = 0;
					
					try{
			    	    Thread.sleep(1000);
			    	}catch(InterruptedException e){
			    	}
				}
			}
		}.start();
	}
	
	public boolean isConnected(){
		return xbee.isConnected();
	}
	
	public void open(String port, int baud) throws IOException, XBeeException {
		open(port, baud, false);
	}

	public void open(String port, int baud, boolean flowControl) throws IOException, XBeeException {
		if(!isConnected()){
			this.port = port;
			this.baud = baud;
			this.flowControl = flowControl;

			try {
				log.info("Starting xbee on port: '" + port + "' with baud: '" + baud + "'");
				RxTxSerialComm serial = new RxTxSerialComm(); 
				serial.openSerialPort(port, "XBee", 1000, baud, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE,
										flowControl ? SerialPort.FLOWCONTROL_RTSCTS_IN | SerialPort.FLOWCONTROL_RTSCTS_OUT : SerialPort.FLOWCONTROL_NONE);
				xbee.initProviderConnection(serial);
				xbee.addPacketListener(this);
				doConnectCallback();
			} catch (XBeeException e) {
				log.error("XBee failed to connect", e);
				close();
				throw e;
			} catch (Exception e) {
				log.error("XBee failed to connect", e);
				close();
				throw new IOException(e);
			}
		}else{
			throw new IllegalStateException("Cannot open new connection -- existing connection is still open.  Please close first");
		}
	}
	
	public void close() {
		try{
			xbee.close();
			doConnectCallback();
		}catch(Exception e){}
		tcpList.clear();
//		xbee.removePacketListener(this);
	}
	
	public int getBaud(){
		return baud;
	}
	
	public String getPort(){
		return port;
	}
	
	public boolean getFlowControl(){
		return flowControl;
	}
	
	/**
	 * Converts a string to a int[] (XBee dislikes char[]..)
	 * @param s - String to convert
	 * @return int[] of the string
	 */
	public static int[] getInts(String s){
		char[] c = s.toCharArray();
		int[] i = new int[c.length];
		for(int o=0; o<c.length;o++){
			i[o] = c[o];
		}
		return i;
	}
	
	public static int[] getInts(byte[] c){
		int[] i = new int[c.length];
		for(int o=0; o<c.length;o++){
			i[o] = c[o];
		}
		return i;
	}
	
	private void checkConnected() throws XBeeException{
		if(!isConnected()){
			throw new XBeeException("Not connected");
		}
	}
	
	public void send(ZNetTxRequest request) throws XBeeTimeoutException, XBeeException{
		send(request, false);
	}
	
	public void send(ZNetTxRequest request, final boolean tcp) throws XBeeTimeoutException, XBeeException{
		checkConnected();
		log.trace("sending tx packet: " + request.toString());
		
		if(request.getDestAddr16().equals(XBeeAddress16.ZNET_BROADCAST)){//No 16bit address set
			XBeeAddress16 target = addresses.get(request.getDestAddr64().toString());
			if(target != null){
				request.setDestAddr16(target);
			}
		}
		
		if(tcp){
			int[] d = request.getPayload();
			int[] n = new int[d.length+2];
			for(int i=0;i<d.length;i++){
				n[i+2] = d[i];
			}
			n[0] = 't';
			n[1] = tcpID.getAndIncrement() % 256;
			request.setPayload(n);
			synchronized (tcpList) {
				tcpList.add(new TCP(request));
			}
		}
		
		request.setFrameId(xbee.getNextFrameId());
		synchronized(sent){
			sent.add(new Sent(request.getFrameId(), request.getDestAddr64(), request.getDestAddr16()));
		}
		final ZNetTxRequest r = request;
		final long scheduled = System.currentTimeMillis(); 
		syncWaiting.getAndIncrement();
		executor.submit(new Runnable(){
			public void run(){
				try {
					if(tcp || System.currentTimeMillis() - scheduled < 1000){
						xbee.sendAsynchronous(r);
						speedOut2 += r.getPayload().length;
					}
				} catch (XBeeException e) {
					log.warn("", e);
					e.printStackTrace();
				}finally{
					syncWaiting.decrementAndGet();
				}
			}
		});
	}
	
	public void send(XBeeAddress64 a64, String data) throws XBeeTimeoutException, XBeeException {
		send(a64, data, false);
	}
	
	public void send(XBeeAddress64 a64, String data, boolean tcp) throws XBeeTimeoutException, XBeeException {
		checkConnected();
		log.trace("sending \"" + data + "\"");
		send(new ZNetTxRequest(a64, getInts(data)), tcp);
	}
	
	public void send(XBeeAddress64 a64, int[] data) throws XBeeTimeoutException, XBeeException {
		send(a64, data, false);
	}
	
	public void send(XBeeAddress64 a64, int[] data, boolean tcp) throws XBeeTimeoutException, XBeeException {
		checkConnected();
		StringBuffer b = new StringBuffer();
		for(int i=0;i<data.length;i++){
			b.append((char)data[i]);
		}
		log.trace("sending \"" + b + "\" length:" + data.length);
		send(new ZNetTxRequest(a64, data), tcp);
	}
	
	public void send(XBeeAddress64 a64, byte[] data) throws XBeeTimeoutException, XBeeException {
		send(a64, data, false);
	}
	
	public void send(XBeeAddress64 a64, byte[] data, boolean tcp) throws XBeeTimeoutException, XBeeException {
		send(a64, getInts(data), tcp);
	}
	
	public void send(AtCommand request) throws XBeeTimeoutException, XBeeException {
		checkConnected();
		log.trace("sending AT packet: " + request.toString());
		
		final AtCommand r = request;
		syncWaiting.incrementAndGet();
		executor.submit(new Runnable(){
			public void run(){
				try {
					xbee.sendAsynchronous(r);
				} catch (XBeeException e) {
					log.warn("", e);
					e.printStackTrace();
				}finally{
					syncWaiting.decrementAndGet();
				}
			}
		});
	}
	
	public void removeTCP(int id){
		synchronized (tcpList) {
			for(Iterator<TCP> it = tcpList.iterator();it.hasNext();){
				TCP t = it.next();
				if(t.id == id){
					it.remove();
					break;
				}
			}
		}
	}
	
	public long getPacketCount(){
		//TODO fix getPacketCount()
		return 0;//xbee.getPacketCount();
	}
	
	/*
	 * Returns the RSSI of the last received trip.
	 * Remember rssi is a negative db value
	 */
	public int getRSSI() throws XBeeException{
		checkConnected();
		return lastRSSI;
	}
	
	public void addPacketListener(PacketListener l) {
		xbee.addPacketListener(l);
	}
	
	public void removePacketListener(PacketListener l) {
		xbee.removePacketListener(l);
	}
	
	private void checkNode(ZNetNode nd){
		ZNetNode existing = null;
		for(ZNetNode n : getNodes()){
			if(n.getNodeAddress64().equals(nd.getNodeAddress64())){
				existing = n;
				break;
			}
		}
		
		if(existing == null){//New node
			nodes.add(nd);
			log.info("New node found: " + nd);
		}else{//Update
			if(nd.getNodeIdentifier() != null){
				nodes.remove(existing);
				nodes.add(nd);
				log.info("Node updated: " + nd);
			}
		}
	}
	
	private void checkNode(ZBNodeDiscover nd){
		checkNode(new ZNetNode(nd));
	}
	
	private void checkNode(ZNetNodeIdentificationResponse ni){
		checkNode(new ZNetNode(ni));
	}
	
	public ZNetNode getNode(XBeeAddress64 a64){
		for(ZNetNode n : getNodes()){
			if(n.getNodeAddress64().equals(a64)){
				return n;
			}
		}
		return null;
	}
	
	public ZNetNode getNode(XBeeAddress16 a16){
		for(ZNetNode n : getNodes()){
			if(n.getNodeAddress16().equals(a16)){
				return n;
			}
		}
		return null;
	}
	
	public ZNetNode getNode(String indentifier){
		for(ZNetNode n : getNodes()){
			if(n.getNodeIdentifier().equals(indentifier)){
				return n;
			}
		}
		return null;
	}
	
	public List<ZNetNode> getNodes(){
		return Collections.unmodifiableList(nodes);
	}
	
	private void checkAddress(XBeeAddress64 a64, XBeeAddress16 a16){
		XBeeAddress16 existing = addresses.get(a64.toString());
		if(existing == null){//New node address
			addresses.put(a64.toString(), a16);
			log.info("New node address found: " + a64 + " : " + a16);
			checkNode(new ZNetNode(a64, a16));
		}else if(! existing.equals(a16)){//Address changed
			addresses.put(a64.toString(), a16);
		}
	}
	
	private boolean deviceParse(ZNetRxResponse response){
		synchronized (Devices.getDevices()){
			for(Device d : Devices.getDevices()){
				if(response.getRemoteAddress64().equals(d.getAddress())){
					d.processResponse(response);
					return true;
				}
			}
		}
		return false;
	}
	
	@Override
	public void processResponse(XBeeResponse response) {
		log.trace("received response " + response.toString());
		
		if(!response.isError()){
			switch(response.getApiId()){
				case ZNET_RX_RESPONSE:
					ZNetRxResponse rxResponse = (ZNetRxResponse) response;
					if(System.currentTimeMillis() - RSSIcooldown >= 1000){
						RSSIcooldown = System.currentTimeMillis();
						new Thread(){
							public void run(){
								try {
									AtCommand at = new AtCommand("DB");
									send(at);
								} catch (XBeeException e) {
									// TODO Auto-generated catch block
									e.printStackTrace();
								}
							}
						}.start();
					}
					
					speedIn2 += rxResponse.getData().length;
					
					checkAddress(rxResponse.getRemoteAddress64(), rxResponse.getRemoteAddress16());
					if(!deviceParse(rxResponse)){
						log.warn("Somone unknown sent us data: " + rxResponse);
					}
					
					break;
					
				case AT_RESPONSE:
					AtCommandResponse atResponse = ((AtCommandResponse)response);
					String cmd = atResponse.getCommand();
					if(cmd.equals("DB")){//RSSI of last received packet (last hop only)
						lastRSSI = -atResponse.getValue()[0];
						if(Settings.getBol("xbeeLogRSSI")){
							log.info("RSSI of last response is " + lastRSSI);
						}
					}else if(cmd.equals("ND")){//Node Discover response
						ZBNodeDiscover nd = ZBNodeDiscover.parse((AtCommandResponse)response);
						checkNode(nd);
						checkAddress(nd.getNodeAddress64(), nd.getNodeAddress16());
					}
					
					break;
				case ZNET_IO_NODE_IDENTIFIER_RESPONSE:
					ZNetNodeIdentificationResponse idResponse = ((ZNetNodeIdentificationResponse)response);
//					if(SourceAction.JOINING.equals(idResponse.getSourceAction())){
						log.info("Node has joined the network:" + idResponse);
//					}
					checkNode(idResponse);
					break;
				case MODEM_STATUS_RESPONSE:
					ModemStatusResponse m = (ModemStatusResponse)response;
					switch(m.getStatus()){
						case ASSOCIATED:
						case COORDINATOR_REALIGNMENT:
						case COORDINATOR_STARTED:
						case DISASSOCIATED:
						case HARDWARE_RESET:
						case SYNCHRONIZATION_LOST:
						case WATCHDOG_TIMER_RESET:
							log.info("Our XBee sent :" + m.getStatus());
					}
					break;
				case ZNET_TX_STATUS_RESPONSE:
					ZNetTxStatusResponse r = (ZNetTxStatusResponse) response;
					if(r.getDeliveryStatus() != ZNetTxStatusResponse.DeliveryStatus.SUCCESS){
						log.warn("Last TX did not get successfully transfered: " + r.getDeliveryStatus() + ", " + r.getDiscoveryStatus());
						sendError++;
					}else{
						synchronized(sent){
							for(Iterator<Sent> it = sent.iterator(); it.hasNext();){
								Sent s = it.next();
								if(s.frameId == r.getFrameId()){
									checkAddress(s.a64, r.getRemoteAddress16());
									it.remove();
									break;
								}
							}
						}
					}
					break;
				case REMOTE_AT_RESPONSE:
					break;
				default:
					log.warn("Recieved packet with unknown/unwanted ApiId: " + response.getApiId());
					break;
			}
	}else{
		ErrorResponse e = (ErrorResponse) response;
		receiveError++;
		log.warn("XBee error: " + e.getException().getMessage());
	}
	}
}


