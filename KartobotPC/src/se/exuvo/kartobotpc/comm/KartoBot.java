package se.exuvo.kartobotpc.comm;

import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;

import se.exuvo.kartobotpc.Init;
import se.exuvo.kartobotpc.Settings;
import se.exuvo.kartobotpc.commands.Parser;
import se.exuvo.kartobotpc.gui.GUI;
import se.exuvo.kartobotpc.gui.Main;

import com.rapplogic.xbee.api.XBeeAddress64;
import com.rapplogic.xbee.api.zigbee.ZNetRxResponse;

public class KartoBot extends Device{
	public long errors = 0;
	public XBeeAddress64 address;	//quad = 00 13 A2 00 40 5C 2C C9
	public Queue<SonarPing> sonar = new LinkedBlockingQueue<SonarPing>(100);
	public Object lock = new Object();
	public int x;
	public int y;
	public Order order;
	public enum Order {
		STOP(0), FORWARD(1), BACK(2), ROTATE(3);
		int id;
		
		Order(int id){
			this.id = id;
		}
		
		static Order getOrder(int id){
			if(id == STOP.id){
				return Order.STOP;
				
			}else if(id == FORWARD.id){
				return Order.FORWARD;
				
			}else if(id == BACK.id){
				return Order.BACK;
				
			}else if(id == ROTATE.id){
				return Order.ROTATE;
				
			}else{
				return null;
			}
		}
	}
	public int throttleL;
	public int throttleR;
	public float speedL;
	public float speedR;
	public float setHeading;
	public float roll;
	public float pitch;
	public float yaw;
	public float heading;
	public float batteryU;
	public float batteryI;
	public PowerSource powerSource = PowerSource.UNKNOWN;
	public enum PowerSource {
		NONE(0), USB(1), BATTERY(2), BATTERY_LOW(3), UNKNOWN(4);
		int id;
		
		PowerSource(int id){
			this.id = id;
		}
		
		static PowerSource get(int id){
			if(id == NONE.id){
				return PowerSource.NONE;
				
			}else if(id == USB.id){
				return PowerSource.USB;
				
			}else if(id == BATTERY.id){
				return PowerSource.BATTERY;
				
			}else if(id == BATTERY_LOW.id){
				return PowerSource.BATTERY_LOW;
				
			}else if(id == UNKNOWN.id){
				return PowerSource.UNKNOWN;
				
			}else{
				return null;
			}
		}
	}
	public int[][] map = new int[128][128];
	public List<Square> path = new ArrayList<Square>(20);
	public boolean dirtyMap = true, dirtyPath = true;
	public int pathTime = 0;
	public int pathLength = 0;
	public List<Task> tasks = new ArrayList<Task>(16);
	public PID[] PIDs = new PID[0];
	
	
	public KartoBot(){
		super("KartoBot");
		try{
			address = new XBeeAddress64(Settings.getStr("remoteAddress64"));
		}catch(Throwable e){
			log.error("Invalid XBee target address: \"" + Settings.getStr("remoteAddress64") + "\"");
			System.exit(2);
		}
	}
	
	public void processResponse(ZNetRxResponse response) {
		int next = 0;
		int[] data = response.getData();
		
		switch(data[next++]){//Command
			case 't':{//TCP
				int id = data[next++];
				log.trace("Quad has recieved TCP: " + id);
				Init.getXConnection().removeTCP(id);
				break;
			}
			
			case 'B':{//Boot
				Parser.parse("clear map");
				Parser.parse("clear path");
				Parser.parse("clear pings");
				log.info("Kartobot booted");
				((Main)Init.getGUI().getState(GUI.MAINSTATE)).console2Append("\nKartobot booted\n");
				break;
			}
			
			case 'e'://Error message
				if(Settings.getBol("logSensors")){
					Init.logSensor("remote error", data[next]);
				}
				String error = "Unknown error type:";
				switch(data[next++]){
					case 12://CHECKSUM_FAILURE
						error = "Checksum failure.";
						break;
					case 13://PACKET_EXCEEDS_BYTE_ARRAY_LENGTH
						error = "Packet exceeds byte array length.";
						break;
					case 14://UNEXPECTED_START_BYTE
						error = "Unexpected start byte.";
						break;
					case 15:
						error = "Unknown command received";
						break;
						
					default:
						error += data[next-1];
				}
				errors++;
				log.warn("Remote got error: " + error);
				break;
				
			case 'p'://Ping reply
				pingReceived();
				break;
				
			case 'P':{//Position update
				synchronized(lock){
					x = readInt16(data[next++], data[next++]);
					y = readInt16(data[next++], data[next++]);
					order = Order.getOrder(data[next++]);
					throttleL = readInt16(data[next++], data[next++]);
					throttleR = readInt16(data[next++], data[next++]);
					speedL = readInt16(data[next++], data[next++]) / 16f;
					speedR = readInt16(data[next++], data[next++]) / 16f;
					setHeading = readInt16(data[next++], data[next++]) / 64f;
					log.trace("Received position update X:" + x + " Y:" + y);
				}
				break;}
			
			case 'A':{//AHRS update
				synchronized(lock){
					roll = readInt16(data[next++], data[next++]) / 64f;
					pitch = readInt16(data[next++], data[next++]) / 64f;
					yaw = readInt16(data[next++], data[next++]) / 64f;
					heading = readInt16(data[next++], data[next++]) / 64f;
					log.trace("Received AHRS update Roll:" + roll + " Pitch:" + pitch + " Yaw:" + yaw + " Heading:" + heading);
				}
				break;}
			
			case 'b':{//Battery update
				synchronized(lock){
					batteryU = readUInt16(data[next++], data[next++]) / 100f;
					batteryI = readInt16(data[next++], data[next++]) / 10f;
					powerSource = PowerSource.get(data[next++]);
					log.trace("Received battery update U:" + batteryU + "V I:" + batteryI + "A" + " source:" + powerSource);
				}
				break;}
				
			case 'c':{//Console
				StringBuffer b = new StringBuffer();
				while(next < data.length){
					b.append((char)data[next++]);
				}
				String s = b.toString();
				log.trace("Received console text[" + s.length() + "]:" + s);
				((Main)Init.getGUI().getState(GUI.MAINSTATE)).console2Append(s);
				break;}
			
			case 's':{//Sonar ping
				int x = readInt16(data[next++], data[next++]);
				int y = readInt16(data[next++], data[next++]);
				sonar.offer(new SonarPing(x, y));
				
				log.trace("Received sonar pulse X:" + x + " Y:"+y);
				break;}
			
			case 'w':{//Warning/Error
				StringBuffer b = new StringBuffer();
				while(next < data.length){
					b.append((char)data[next++]);
				}
				String s = b.toString();
				log.warn("Received warning text[" + s.length() + "]:" + s);
				((Main)Init.getGUI().getState(GUI.MAINSTATE)).console2Append(s);
				break;}
			
			case 'n':{//Navigation grid
				synchronized(map) {
					while(next < data.length){
						int x = data[next++];
						int y = data[next++];
						map[x][y] = data[next++];
						log.trace("Received grid part: X:" + x + " Y:" + y + " :" + map[x][y]);
					}
				}
				
				dirtyMap = true;
				break;
			}
			
			case 'N':{//Navigation path
				pathTime = readUInt16(data[next++], data[next++]);
				pathLength = readInt16(data[next++], data[next++]);
				synchronized(path) {
					path.clear();
					while(next < data.length){//First point is goal
						int x = data[next++];
						int y = data[next++];
						path.add(new Square(x, y));
					}
				}
				
				log.trace("Received path: steps:" + path.size());
				dirtyPath = true;
				break;
			}
			
			case 'S':{//Task status
				log.trace("Got task status");
				synchronized(tasks) {
					while(next < data.length){
						int id = data[next++];
						int time = readUInt16(data[next++], data[next++]);
						int totalP = (int) (data[next++] / 2.55f);
						
						Task t = getTask(id);
						if(t != null){
							t.totalP = totalP;
							t.time = time;
						}
					}
					updateTasks();
				}
				break;
			}
			
			case 'i':{//Tasks status init
				log.trace("Got task init");
				synchronized(tasks) {
//					tasks.clear();
					while(next < data.length){
						int id = data[next++];
						if(id == 0xFF && next == data.length){
							tasks.clear();
							break;
						}
						
						StringBuilder b = new StringBuilder(16);
						while(true){
							int c = data[next++];
							if(c > 0){
								b.append((char)c);
							}else{
								break;
							}
						}
						tasks.add(new Task(id, b.toString()));
					}
				}
				break;
			}
			
			case 'd':{//PIDs
				switch(data[next++]){
					case 'g':{//Get
						int id = data[next++];
						synchronized(PIDs){
							if(PIDs[id] == null) PIDs[id] = new PID(id);
							PIDs[id].P = readInt24(data[next++], data[next++], data[next++]) / 1024f;
							PIDs[id].I = readInt24(data[next++], data[next++], data[next++]) / 1024f;
							PIDs[id].D = readInt24(data[next++], data[next++], data[next++]) / 1024f;
							StringBuilder build = new StringBuilder(16);
							while(next < data.length && data[next] != 0){
								build.append((char) data[next++]);
							}
							PIDs[id].name = build.toString();
							log.info("PID " + id + " received: \"" + PIDs[id].name + "\"");
						}
						((Main)Init.getGUI().getState(GUI.MAINSTATE)).reloadPids.set(true);
						break;
					}
					case 'l':{//Length
						int length = data[next++];
						log.info("PID array length = " + length);
						synchronized(PIDs){
							PIDs = new PID[length];
						}
						
						for(int i=0; i < length; i++){
							getC().send(new int[]{'P', 'g', i});
						}
						break;
					}
				}
				break;
			}
			
			default:
				log.warn("Unknown command from remote: " + data[next]);
		}
	}

	private void updateTasks(){
		for(Task t : tasks){
			t.P = (int) t.time / 100;
		}
	}

	@Override
	public XBeeAddress64 getAddress() {
		return address;
	}
	
	private Task getTask(int id){
		for(Task t : tasks){
			if(t.id == id){
				return t;
			}
		}
		return null;
	}
	
	public static class SonarPing{
		public final int x,y;
		public final long time;
		
		public SonarPing(int x, int y){
			this.x = x;
			this.y = y;
			time = System.currentTimeMillis();
		}
		
		public String toString(){
			return "X:" + x + " Y:" + y;
		}
	}
	
	public static class Point{
		public int x,y;
		
		public Point(int x, int y){
			this.x = x;
			this.y = y;
		}
		
		public String toString(){
			return "X:" + x + " Y:" + y;
		}
	}
	
	public static class Square{
		public int x,y;
		
		public Square(int x, int y){
			this.x = x;
			this.y = y;
		}
		
		public String toString(){
			return "X:" + x + " Y:" + y;
		}
	}
	
	public static class Task{
		public int time, totalP, P, id;
		public String name;
		
		public Task(int id, String name){
			this.id = id;
			this.name = name;
		}
	}
	
	public static class PID{
		public float P, I, D;
		public int id;
		public String name;
		
		public PID(int id){
			this.id = id;
		}
	}
	
}



