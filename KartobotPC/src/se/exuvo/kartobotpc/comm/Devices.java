package se.exuvo.kartobotpc.comm;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.log4j.Logger;

import se.exuvo.kartobotpc.ClassFinder;

public class Devices {
	private static final Logger log = Logger.getLogger(Devices.class);
	private static final List<Device> devices = new ArrayList<Device>();
	private static List<Class<? extends Device>> types = new ArrayList<Class<? extends Device>>();
	
	public static void init(){
		types.clear();
		log.debug("Loading device types");
		try {
			List<Class<?>> l = ClassFinder.getClasses("se.exuvo.kartobotpc.comm");
			for(Class<?> c : l){
				if(Device.class.isAssignableFrom(c) && !c.equals(Device.class)){
					try {
						Class<? extends Device> cc = c.asSubclass(Device.class);
						types.add(cc);
						log.trace("Loaded device type: " + cc.getSimpleName());
					} catch (Throwable e) {
						log.warn("Failed to load device type: \"" + c.getSimpleName() + "\"", e);
					} 
				}
			}
		} catch (ClassNotFoundException e) {
			log.warn("Failed to load commands", e);
		} catch (IOException e) {
			log.warn("Failed to load commands", e);
		}
	}

	/**
	 * You MUST synchronise on this before using.
	 * @return
	 */
	public static List<Device> getDevices(){
		return devices;
	}
	
	public static void addDevice(Device d){
		synchronized(devices){
			devices.add(d);
		}
	}
	
	public static boolean addDevice(String type){
		synchronized(types){
			for(Class<? extends Device> c : types){
				if(c.getSimpleName().equalsIgnoreCase(type)){
					try {
						Device d = c.newInstance();
						addDevice(d);
						log.debug("Added device: " + d.getName());
						return true;
					} catch (Throwable e) {
						log.warn("Failed to create device: \"" + c.getSimpleName() + "\"", e);
					} 
				}
			}
		}
		return false;
	}
	
	public static Device getDevice(String type){
		synchronized(devices){
			for(Device d : devices){
//				if(d.getName().equals(type)){
				if(d.getClass().getSimpleName().equalsIgnoreCase(type)){
					return d;
				}
			}
		}
		return null;
	}
}
