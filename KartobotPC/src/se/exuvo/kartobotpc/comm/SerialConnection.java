package se.exuvo.kartobotpc.comm;

import gnu.io.SerialPort;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

import org.apache.log4j.Logger;

import se.exuvo.kartobotpc.commands.Parser;

import com.rapplogic.xbee.RxTxSerialComm;
import com.rapplogic.xbee.api.XBeeException;
import com.rapplogic.xbee.api.XBeePacket;
import com.rapplogic.xbee.api.zigbee.ZNetRxResponse;
import com.rapplogic.xbee.util.ByteUtils;

import de.matthiasmann.twl.utils.CallbackSupport;


/*
 * Packet format = id, length, data..
 */
public class SerialConnection implements Runnable{
	private static final Logger log = Logger.getLogger(SerialConnection.class);
	private RxTxSerialComm serial = null;
	public long sendError = 0;
	public long receiveError = 0;
	public AtomicInteger syncWaiting = new AtomicInteger();
	private ScheduledExecutorService executor;
	private Thread thread = null;
	private boolean done = false;
	private String port = "";
	private int baud = 0;
	private boolean flowControl = false;
	public int speedIn = 0;
	public int speedOut = 0;
	private int speedIn2 = 0;
	private int speedOut2 = 0;
	private Device device;
	
	public long lastPing = System.currentTimeMillis();
	public long roundTrip = 0;
	private long pingSent;
	
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
	
	public SerialConnection(){
	}
	
	public Device getConnectedDevice(){
		return device;
	}
	
	public boolean isConnected(){
		try {
			if (serial != null && serial.getInputStream() != null && serial.getOutputStream() != null) {
				return true;
			}else{
				return false;
			}
		} catch (Exception e) {
			return false;
		}
	}
	
	
	
	public void open(String port, int baud) throws IOException, XBeeException {
		open(port, baud, false);
	}

	public void open(String port, int baud, boolean flowControl) throws IOException {
		if(!isConnected()){
			this.port = port;
			this.baud = baud;
			this.flowControl = flowControl;
			syncWaiting.set(0);
			setDone(false);

			try {
				log.info("Starting serial on port: '" + port + "' with baud: '" + baud + "'");
				serial = new RxTxSerialComm(); 
				serial.openSerialPort(port, "KartobotPC", 1000, baud, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE,
										flowControl ? SerialPort.FLOWCONTROL_RTSCTS_IN | SerialPort.FLOWCONTROL_RTSCTS_OUT : SerialPort.FLOWCONTROL_NONE);
				executor = Executors.newSingleThreadScheduledExecutor();
				thread = new Thread(this);
				thread.setName("SerialInputStreamThread");
				thread.start();
				doConnectCallback();
				new Thread(){
					public void run(){
						while(!done){
							speedIn = speedIn2;
							speedIn2 = 0;
							speedOut = speedOut2;
							speedOut2 = 0;
							
							if(!isConnected()){
								close();
								break;
							}
							
							if(device == null){
								try {
									send("?");
								} catch (IOException e) {
									log.warn("", e);
									e.printStackTrace();
								}
							}else{
								try {
									send("p");
									pingSent = System.currentTimeMillis();
								} catch (IOException e) {
									log.warn("", e);
									e.printStackTrace();
								}
								
								if(System.currentTimeMillis() - lastPing > 4000){//Lost connection
									int sec = (int) ((System.currentTimeMillis() - lastPing)/1000);
									log.warn("SerialConnection: \"" + device.getName() + "\" has not responded to ping in " + sec + " seconds!");
									close();
									break;
								}
							}
							
							try{
					    	    Thread.sleep(1000);
					    	}catch(InterruptedException e){
					    	}
						}
					}
				}.start();
			} catch (IOException e) {
				log.error("Serial failed to connect", e);
				close();
				throw e;
			} catch (Exception e) {
				log.error("Serial failed to connect", e);
				close();
				throw new IOException(e);
			}
		}else{
			throw new IllegalStateException("Cannot open new connection -- existing connection is still open.  Please close first");
		}
	}
	
	public void close() {
		try{
			if(serial != null && done == false){
				setDone(true);
				if(thread != null){
					interrupt();
					while(thread.isAlive());
				}
				if(executor != null){
					executor.shutdownNow();
				}
				serial.close();
				serial = null;
				device.usb = null;
				device = null;
				doConnectCallback();
			}
		}catch(Exception e){
			log.warn("", e);
		}
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
	
	private int read(InputStream in) throws IOException{
		int val = in.read();
		if(val == XBeePacket.SpecialByte.ESCAPE.getValue()){
			val = in.read() ^ 0x20;
		}
		return val;
	}
	
	public void run() {
		InputStream in = serial.getInputStream();
		
		try {
			while (!done) {
				try {
					if (in.available() > 0) {
						int val = in.read();
						
						if (val == XBeePacket.SpecialByte.START_BYTE.getValue()) {
							int length = read(in) << 8 | read(in);
							int data[] = new int[length];
							length = 0;
							while(length < data.length){
								data[length++] = read(in);
							}
							
							log.trace("Received packet: int[] packet = {" + ByteUtils.toBase16(data, ", ") + "};");	
							
							final int data2[] = data;
							executor.submit(new Runnable() {
								@Override
								public void run() {
									try{
										processResponse(data2);
									}catch(Throwable t){
										log.warn("", t);
									}
								}
							});
						} else {
							log.warn("expected start byte but got this " + ByteUtils.toBase16(val) + ", discarding");
						}
					} else {
						log.trace("No data available.. waiting for new data event");
						
						// we will wait here for RXTX to notify us of new data
						synchronized (serial) {
							// There's a chance that we got notified after the first in.available check
							if (in.available() > 0) {
								continue;
							}
							
							// wait until new data arrives
							serial.wait();
						}	
					}				
				} catch (Exception e) {
					if (e instanceof InterruptedException) throw ((InterruptedException)e);
					
					log.error("Error while parsing packet:", e);
					
					if (e instanceof IOException) {
						// this is thrown by RXTX if the serial device unplugged while we are reading data; if we are waiting then it will waiting forever
						log.error("Serial device IOException.. exiting");
						break;
					}
				}
			}
		} catch(InterruptedException ie) {
			// We've been told to stop -- the user called the close() method			
			log.trace("Packet parser thread was interrupted.  This occurs when close() is called");
		} finally {
			if (serial != null) {
				serial.close();
			}
		}
		
		log.info("InputStreamThread is exiting");
	}

	public void setDone(boolean done) {
		this.done = done;
	}
	
	public void send(String s) throws IOException{
		log.trace("sending \"" + s + "\"");
		send(XbeeConnection.getInts(s));
	}
	
	public void send(final int data[]) throws IOException{
		checkConnected();
		log.trace("sending packet: " + ByteUtils.toBase16(data));
		
		final long scheduled = System.currentTimeMillis(); 
		syncWaiting.getAndIncrement();
		executor.submit(new Runnable(){
			public void run(){
				try {
					if(System.currentTimeMillis() - scheduled < 1000){
						OutputStream out = serial.getOutputStream();
						for(int i=0; i<data.length; i++){
							out.write(data[i]);
						}
						out.flush();
						speedOut2 += data.length;
					}
				} catch (IOException e) {
					log.warn("", e);
					e.printStackTrace();
				}finally{
					syncWaiting.decrementAndGet();
				}
			}
		});
	}
	
	public void interrupt() {
		if (thread != null) {
			try {
				thread.interrupt();	
			} catch (Exception e) {
				log.warn("Error interrupting parser thread", e);
			}
		}
	}
	
	private void checkConnected() throws IOException{
		if(!isConnected()){
			throw new IOException("Not connected");
		}
	}
	
	public void processResponse(int data[]) {
		log.trace("received response \"" + ByteUtils.toString(data) + "\" - " + ByteUtils.toBase16(data));
		speedIn2 += data.length;
		lastPing = System.currentTimeMillis();
		
		int n = 0;
		switch(data[n++]){
			case '?':{//Device identification
				StringBuilder build = new StringBuilder(16);
				while(n < data.length && data[n] != 0){
					build.append((char) data[n++]);
				}
				String type = build.toString();
				
				device = Devices.getDevice(type);
				if(device == null){
					if(Devices.addDevice(type)){
						device = Devices.getDevice(type);
					}
				}
				
				if(device != null){
					device.usb = this;
					log.info("New serial device of type \"" + device.getName() + "\" connected!");
					Parser.parse("echo " + "New serial device of type \"" + device.getName() + "\" connected!\n");
					try {
						send("!");
					} catch (IOException e) {
						log.warn("", e);
						e.printStackTrace();
					}
				}else{
					log.warn("Unknown device type connected: " + type);
				}
				break;
			}
			case '!':{//Device authorised
				
				break;
			}
			case 'p':{//Ping
				roundTrip = System.currentTimeMillis() - pingSent;
				log.trace("SerialConnection: \"" + device.getName() + "\" responded to ping after " + roundTrip + "ms");
			}
			default:
				if(device != null){
					ZNetRxResponse response = new ZNetRxResponse();
					response.setData(data);
					device.processResponse(response);
				}
				break;
		}
	}
	
}


