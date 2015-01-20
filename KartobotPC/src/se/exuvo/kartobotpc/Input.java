package se.exuvo.kartobotpc;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.EventListener;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;

import javax.swing.event.EventListenerList;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactoryConfigurationError;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import net.java.games.input.Component;
import net.java.games.input.Controller;
import net.java.games.input.ControllerEnvironment;
import net.java.games.input.Event;

import org.apache.log4j.Logger;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import se.exuvo.kartobotpc.comm.Devices;
import se.exuvo.kartobotpc.comm.XbeeConnection;
import se.exuvo.kartobotpc.commands.Command;
import se.exuvo.kartobotpc.commands.Parser;
import se.exuvo.settings.Setting;
import se.unlogic.standardutils.xml.XMLUtils;

import com.rapplogic.xbee.api.XBeeAddress64;
import com.rapplogic.xbee.api.XBeeException;
import com.rapplogic.xbee.api.XBeeTimeoutException;


public class Input extends Thread{
	private static final Logger log = Logger.getLogger(Input.class);
	private List<Bind> binds =  Collections.synchronizedList(new ArrayList<Bind>());
	private EventListenerList listeners = new EventListenerList();
	private static HashMap<String, Float> vars = new HashMap<String, Float>();

	public List<Controller> getControllers(){
		ControllerEnvironment ce = ControllerEnvironment.getDefaultEnvironment();
		Controller[] controllers = ce.getControllers();
		return Arrays.asList(controllers);
	}
	
	public List<Bind> getBinds(){
		return binds;
	}
	
	 public void addEventListener(InputListener l) {
	     listeners.add(InputListener.class, l);
	 }

	 public void removeEventListener(InputListener l) {
	     listeners.remove(InputListener.class, l);
	 }

	 protected void fireEventListeners(Event e) {
	     // Guaranteed to return a non-null array
	     Object[] listen = listeners.getListenerList();
	     // Process the listeners last to first, notifying
	     // those that are interested in this event
	     for (int i = listen.length-2; i>=0; i-=2) {
	         if (listen[i]==InputListener.class) {
	             ((InputListener)listen[i+1]).inputChanged(e);
	         }
	     }
	 }
	 
	public Input(){
		for(Iterator<Controller> it = getControllers().iterator();it.hasNext();){
			Controller c = it.next();
			Component[] cc = c.getComponents();
			
			log.debug("Controller name: '" + c.getName() + "' type:'" + c.getType() + "' components:" + cc.length);
		}
		
		load();
		this.start();
	}
	
	public Controller getController(Component c){
		for(Controller cc : getControllers()){
			for(Component ccc : cc.getComponents()){
				if(c == ccc){
					return cc;
				}
			}
		}
		return null;
	}
	
	public void addBind(Component c, String prefix){
		Bind b = new Bind(getController(c), c);
		b.prefix = prefix;
		synchronized(binds){
			binds.add(b);
		}
	}
	
	public void addBind(Bind b){
		synchronized(binds){
			binds.add(b);
		}
	}
	
	byte[] data = new byte[0];
	private void append(byte[] newData){
		Init.getXConnection();
		if(data.length + newData.length > XbeeConnection.maxPacketDataSize){
			send();
		}
		
		int oldSize = data.length;
		data = Arrays.copyOf(data, data.length + newData.length);
		for(int i=0;i<newData.length;i++){
			data[oldSize+i] = newData[i];
		}
	}
	
	private void send(){
		if(!Init.getXConnection().isConnected()){
			return;
		}
		try {
			if(data.length > 0){
				Init.getXConnection().send(Devices.getDevices().get(0).getAddress(), data);
			}
		} catch (XBeeTimeoutException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (XBeeException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		data = new byte[0];
	}
	
	public void run(){
		while(true){
			for(Iterator<Controller> it = getControllers().iterator();it.hasNext();){
				Controller c = it.next();
				c.poll();
				Event e = new Event();
				while(c.getEventQueue().getNextEvent(e)){
					//System.out.println("name: '" + e.getComponent().getName() + "' value:'" + e.getValue() + "'");
					
					synchronized(binds){
						for(Iterator<Bind> it2=binds.iterator();it2.hasNext();){
							Bind b = it2.next();
							if(b.controllerType.equals(c.getType().toString()) && b.component.equals(e.getComponent().getName())){
								if(!b.code.equals("")){
									b.doStuff(e.getValue());
								}else{
									int value = b.scale(e.getValue());
									if(!b.isBoolean){
										if(System.currentTimeMillis() - b.lastSendTime > 100){//Don't spam
											byte[] data = Arrays.copyOf(b.prefix.getBytes(), b.prefix.getBytes().length +2);
											data[data.length-2] = (byte) (value >> 8 );
											data[data.length-1] = (byte) (value & 0xFF);
											append(data);
											b.lastSendTime = System.currentTimeMillis();
											b.skipped = false;
										}else{
											b.skipped = true;
										}
									}else if(value > 0){
										Parser.parse("send " + b.prefix);
									}
								}
							}
						}
					}
					fireEventListeners(e);
				}
			}
			for(Bind b : binds){
				//Update old value, fix for packet drop
				//Shorter delay for when you stop moving a Component to send the final value quickly
				if(b.c != null && b.code.equals("") && !b.isBoolean && (System.currentTimeMillis() - b.lastSendTime > 1000 ||
								   (System.currentTimeMillis() - b.lastSendTime > 100 && b.skipped))){
					int value = b.scale(b.c.getPollData());
					byte[] data = Arrays.copyOf(b.prefix.getBytes(), b.prefix.getBytes().length +2);
					data[data.length-2] = (byte) (value >> 8 );
					data[data.length-1] = (byte) (value & 0xFF);
					append(data);
					b.lastSendTime = System.currentTimeMillis();
					b.skipped = false;
				}
			}
			send();
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
			}
		}
	}
	
	float map(float x, float in_min, float in_max, float out_min, float out_max)
	{
	  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	public class Bind{
		public String controllerType = "";
		public String component = "";
		public Component c = null;
		public String prefix = "";
		public String name = "";
		public float in_min = -1;
		public float in_max = 1;
		public float deadzone = 0;
		public int out_min = 0;
		public int out_max = 1000;
		public int trim = 0;
		/** If true, sends only prefix when scale returns >0 */
		public boolean isBoolean = false;
		public boolean isToggle = false;//TODO implement
		public long lastSendTime = 0;
		public boolean skipped = false;
		public String code = "";
		
		public Bind(Controller c, Component cc){
			this.controllerType = c.getType().toString();
			this.component = cc.getName();
			this.c = cc;
		}
		
		public Bind(String controllerType, String componentName){
			this.controllerType = controllerType;
			this.component = componentName;
		}
		
		public int scale(float in){
//			float center = in_max - in_min;
//			if(in - deadzone > center){
//				in -= deadzone;
//			}else if(in + deadzone < center){
//				in += deadzone;
//			}else{
//				in = center;
//			}
			return Math.min(out_max, Math.max(out_min, trim + (int) map(in, in_min, in_max, out_min, out_max)));
		}
		
		public String toString(){
			return name + ": " + prefix;
		}
		
		public void doStuff(float in){
			String[] tokens = code.split("[ ]+");
			
			int n = 0;
			while(n<tokens.length){
				log.debug(tokens[n]);
				if(tokens[n].equals("eq")){
					if(Float.parseFloat(tokens[++n]) != in){
						break;
					}
				}else if(tokens[n].equals("trim")){
					int add = 0;
					n++;
					if(tokens[n].startsWith("+")){
						add += Integer.parseInt(tokens[n].substring(1));
					}else if(tokens[n].startsWith("-")){
						add -= Integer.parseInt(tokens[n].substring(1));
					}
					
					for(Bind b : getBinds()){
						if(b.prefix.equals(prefix)){
							b.trim += add;
						}
					}
				}else if(tokens[n].length() > 1 && tokens[n].charAt(0) == '$'){
					setVar(tokens[n].substring(1), scale(in));
				}else if(tokens[n].length() > 1 && tokens[n].charAt(0) == '%'){
					getVar(tokens[n].substring(1));//TODO choice for #bits
				}
				n++;
			}
		}
	}
	
	public static void setVar(String key, float value){
		vars.put(key, value);
	}
	
	public static float getVar(String key){
		return vars.get(key);
	}
	
	public interface InputListener extends EventListener{
		public void inputChanged(Event e);
	}
	
	private Element newChild(String name, Object value, String type, Document doc){
		Element e = XMLUtils.createElement(name, value.toString(), doc);
		e.setAttribute("type", type);
		return e;
	}
	
	/**
     * Saves binds list to xml file.
     */
	public boolean save(){
		log.info("Saving Input settings");
		File file = new File("settings.xml");
		Document doc;
		XPath xPath = XPathFactory.newInstance().newXPath();
		
		if(!file.exists()){
			log.warn("Settings file not found!");
			return false;
		}else{
			try {
				doc = XMLUtils.parseXmlFile(file, false, false);
			} catch (SAXException e) {
				log.error("SAXException: " + e + " while parsing " + file);
				return false;
			} catch (IOException e) {
				log.error("IOException: " + e + " while opening "+ file);
				return false;
			} catch (ParserConfigurationException e) {
				log.error("ParserConfigurationException: " + e + " while parsing "+ file);
				return false;
			}
		}
		
		try{
			// Root
			Node root = (Node) xPath.evaluate("/" + Settings.getSectionName(), doc,XPathConstants.NODE);
			if(root == null){
				root = doc.createElement(Settings.getSectionName());
				doc.appendChild(root);
			}
			
			// Binds
			Node bindsElement = (Node) xPath.evaluate("/" + Settings.getSectionName() + "/binds", doc,XPathConstants.NODE);
			if(bindsElement == null){
				bindsElement = doc.createElement("binds");
				root.appendChild(bindsElement);
			}else{
				Node n = doc.createElement("binds");
				root.replaceChild(n, bindsElement);
				bindsElement = n;
			}
			
			/*NodeList nodeList = bindsElement.getChildNodes();
			for(int i=0; i<nodeList.getLength();i++){
				log.debug("removing child;" + nodeList.item(i));
				bindsElement.removeChild(nodeList.item(i));//Clear nodes
			}*/
			
			for (ListIterator<Bind> it = binds.listIterator();it.hasNext();){
				Bind b = it.next();
				log.trace("Saving bind:" +b);
				Element neW = XMLUtils.createElement("bind", "", doc);
				
				if(b.c != null){
					Controller co = getController(b.c);
					neW.appendChild(newChild("controllerType", co.getType(), "s" , doc));
					neW.appendChild(newChild("component", b.c.getName(), "s" , doc));
				}else{
					neW.appendChild(newChild("controllerType", b.controllerType, "s" , doc));
					neW.appendChild(newChild("component", b.component, "s" , doc));
				}
				neW.appendChild(newChild("prefix", b.prefix, "s", doc));
				neW.appendChild(newChild("name", b.name, "s", doc));
				neW.appendChild(newChild("in_min", b.in_min, "f", doc));
				neW.appendChild(newChild("in_max", b.in_max, "f", doc));
				neW.appendChild(newChild("trim", b.trim, "i", doc));
				neW.appendChild(newChild("out_min", b.out_min, "i", doc));
				neW.appendChild(newChild("out_max", b.out_max, "i", doc));
				neW.appendChild(newChild("isBoolean", b.isBoolean, "b", doc));
				if(!b.code.equals("")){
					neW.appendChild(newChild("code", b.code, "s", doc));
				}
				
				bindsElement.appendChild(neW);
			}
	
			XMLUtils.writeXmlFile(doc, file, true, "UTF-8");
			return true;
		}catch(XPathExpressionException e){
			log.error("XPathExpressionException: " + e + " while writing xml");
			return false;
		} catch (TransformerFactoryConfigurationError e) {
			log.error("TransformerFactoryConfigurationError: " + e + " while writing xml");
			return false;
		} catch (TransformerException e) {
			log.error("TransformerException: " + e + " while writing xml");
			return false;
		}catch(Throwable t){
			log.error("" ,t);
			return false;
		}
			
	}
	
	/**
     * Loads binds from xml file.
     */
	public boolean load(){
		File file = new File("settings.xml");
		
		if(!file.exists()){
			save();
		}
		
		try{
			Document doc = XMLUtils.parseXmlFile(file, false, false);
			XPath xPath = XPathFactory.newInstance().newXPath();
			
			NodeList nodeList = (NodeList) xPath.evaluate("/" + Settings.getSectionName() + "/binds/*", doc,XPathConstants.NODESET);
			for (int i=0; i<nodeList.getLength(); i++){
				try{
					Element e = (Element) nodeList.item(i);
					Hashtable<String, Node> values = new Hashtable<String, Node>();
					
					NodeList children = e.getChildNodes();
					for(int o=0;o<children.getLength();o++){
						Node child = children.item(o);
						values.put(child.getNodeName(), child);
					}
					
					Node n = values.get("controllerType");
					String controllerType = new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getStr();
					n = values.get("component");
					String componentName = new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getStr();
					
					Controller c = null;
					Component cc = null;
					for(Controller ccc : getControllers()){
						if(ccc.getType().toString().equals(controllerType)){
							c = ccc;
							break;
						}
					}
					Bind b = null;
					if(c != null){
						for(Component ccc : c.getComponents()){
							if(ccc.getName().equals(componentName)){
								cc = ccc;
								b = new Bind(c, cc);
								break;
							}
						}
					}
					if(cc == null){//ControllerType not found
						log.warn("Bind " + controllerType + "/" + componentName + " not available.");
						b = new Bind(controllerType, componentName);
					}
					if(b != null){
						b.prefix = values.get("prefix").getTextContent();
					}
					
					n = values.get("name");
					b.name = new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getStr();
					n = values.get("in_min");
					b.in_min = new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getFloat();
					n = values.get("in_max");
					b.in_max = new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getFloat();
					n = values.get("trim");
					b.trim = n == null? 0: new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getInt();
					n = values.get("out_min");
					b.out_min = new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getInt();
					n = values.get("out_max");
					b.out_max = new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getInt();
					n = values.get("deadzone");
					b.deadzone = n == null ? 0: new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getFloat();
					n = values.get("isBoolean");
					b.isBoolean = new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getBol();
					n = values.get("code");
					b.code = n == null ? "": new Setting(n.getNodeName() , n.getTextContent(), n.getAttributes().getNamedItem("type").getTextContent().charAt(0)).getStr();
					
					log.trace("Loaded bind:" +b);
					addBind(b);
				}catch(Throwable t){
					log.warn("Exception reading bind. Fix settings.xml", t);
					System.exit(1);
				}
			}
		
			return true;
		} catch (SAXException e) {
			log.warn("SAXException: " + e + " while reading xml");
		} catch (IOException e) {
			log.warn("IOException: " + e + " while reading xml");
		} catch (ParserConfigurationException e) {
			log.warn("ParserConfigurationException: " + e + " while reading xml");
		} catch (XPathExpressionException e) {
			log.warn("XPathExpressionException: " + e + " while reading xml");
		}catch(Throwable t){
			log.error("" ,t);
			return false;
		}
		return false;
	}
	
}
