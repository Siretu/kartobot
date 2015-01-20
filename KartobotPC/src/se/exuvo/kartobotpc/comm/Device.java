package se.exuvo.kartobotpc.comm;

import java.io.IOException;
import java.security.InvalidParameterException;
import java.util.Arrays;

import org.apache.log4j.Logger;

import se.exuvo.kartobotpc.Init;

import com.rapplogic.xbee.api.XBeeAddress64;
import com.rapplogic.xbee.api.XBeeException;
import com.rapplogic.xbee.api.XBeeTimeoutException;
import com.rapplogic.xbee.api.zigbee.ZNetRxResponse;

public abstract class Device {
	protected final Logger log = Logger.getLogger(Device.class);
	private String name;
	public long pings = 0;
	public long lastPing = System.currentTimeMillis();
	public long lostPings = 0;
	public long roundTrip = 0;
	private long pingSent;
	public SerialConnection usb = null;
	
	public static int readInt32(int b1, int b2, int b3, int b4){
		int temp = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
		temp = temp < 0x80000000 ? temp:temp - 0xFFFFFFFF;
		return temp;
	}
	
	public static int readUInt32(int b1, int b2, int b3, int b4){
		int temp = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
		return temp;
	}
	
	public static int readInt24(int b1, int b2, int b3){
		int temp = (b1 << 16) | (b2 << 8) | b3;
		temp = temp < 0x800000 ? temp:temp - 0xFFFFFF;
		return temp;
	}
	
	public static int readUInt24(int b1, int b2, int b3){
		int temp = (b1 << 16) | (b2 << 8) | b3;
		return temp;
	}
	
	public static int readInt16(int msb, int lsb){
		int temp = (msb << 8) | lsb;
		temp = temp < 0x8000 ? temp:temp - 0xFFFF;
		return temp;
	}
	
	public static int readUInt16(int msb, int lsb){
		int temp = (msb << 8) | lsb;
		return temp;
	}
	
	public static float readFloat(int msb, int bit, int lsb){
		int stuff = readInt24(msb, bit, lsb);
		return stuff / 1024.0f;
	}
	
	/**
	 * @param f max value of 8191, min of -8191. Precision of 0.00098.
	 * @return 3 bytes
	 */
	public static int[] writeFloat(float f){
		if(f > 8191 || f < -8191)throw new InvalidParameterException("max/min float value is 8191/-8191!");
		int a = (int) (f * 1024);
		int[] i = new int[3];
		i[2] = ((int)a) & 0xFF;
		i[1] = (((int)a) >> 8) & 0xFF;
		i[0] = (((int)a) >> 16) & 0xFF;
		if(a < 0){
			i[0] |= 0x80;
		}
		return i;
	}
	
	/**
	 * appends the float to array o
	 * @param f float to be appended
	 * @param o target array
	 * @return
	 */
	public static int[] writeFloat(float f, int[] o){
		int[] a = writeFloat(f);
		int[] n = Arrays.copyOf(o, o.length + a.length);
		for(int i=0;i<a.length;i++){
			n[o.length+i] = a[i];
		}
		return n;
	}
	
	public Device(String name){
		this.name = name;
		final Device d = this;
		new Thread(){
			public void run(){
				while(true){
					try{
			    	    Thread.sleep(1000);
			    	}catch(InterruptedException e){
			    	}
				    if(getX().isConnected()){
					  try{//TODO this spams device if device has been disconnected for a while, why?
					      getX().send(getAddress(), "p", false);//Ping keep connection alive
					      pingSent = System.currentTimeMillis();
					  }catch(XBeeException e){
					  }
					  if(System.currentTimeMillis() - lastPing > 1500){
						  lostPings++;
					  }
					  
					  if(System.currentTimeMillis() - lastPing > 4000){//Lost connection
						  int sec = (int) ((System.currentTimeMillis() - lastPing)/1000);
						  if(sec % 60 == 0){
							  log.warn("\"" + d.getName() + "\" has not responded to ping in " + sec + " seconds!");
						  }
					  }
				    }
				}
			}
		}.start();
	}
	
	public String getName(){
		return name;
	}
	
	/**
	 * Please do call this
	 */
	protected void pingReceived(){
		pings++;
		roundTrip = System.currentTimeMillis() - pingSent;
		lastPing = System.currentTimeMillis();
		log.trace("\"" + getName() + "\" responded to ping after " + roundTrip + "ms");
	}
	
	public Connection getC(){
		if(usb != null){
			return new Connection() {
				@Override
				public void send(int[] data) {
					try {
						getS().send(data);
					} catch (IOException e) {
						log.warn("", e);
						e.printStackTrace();
					}
				}
				
				@Override
				public void send(String s) {
					try {
						getS().send(s);
					} catch (IOException e) {
						log.warn("", e);
						e.printStackTrace();
					}
				}
				
				@Override
				public boolean isConnected() {
					return getS().isConnected();
				}
			};
		}else{
			return new Connection() {
				
				@Override
				public void send(int[] data) {
					try {
						getX().send(getAddress(), data, true);
					} catch (XBeeTimeoutException e) {
						log.warn("", e);
						e.printStackTrace();
					} catch (XBeeException e) {
						log.warn("", e);
						e.printStackTrace();
					}
				}
				
				@Override
				public void send(String s) {
					try {
						getX().send(getAddress(), s, true);
					} catch (XBeeTimeoutException e) {
						log.warn("", e);
						e.printStackTrace();
					} catch (XBeeException e) {
						log.warn("", e);
						e.printStackTrace();
					}
				}
				
				@Override
				public boolean isConnected() {
					return getX().isConnected();
				}
			};
		}
	}
	
	protected XbeeConnection getX(){
		return Init.getXConnection();
	}
	
	protected SerialConnection getS(){
		return usb;
	}
	
	protected void connectionReset(){
		lastPing = System.currentTimeMillis();
	}
	
	public abstract void processResponse(ZNetRxResponse response);
	public abstract XBeeAddress64 getAddress();
	
	public static interface Connection {
		
		public boolean isConnected();
		public void send(String s);
		public void send(final int data[]);

	}
}
