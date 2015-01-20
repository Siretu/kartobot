package se.exuvo.kartobotpc;

import gnu.io.CommPortIdentifier;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;
import java.util.List;
import java.util.Scanner;

import org.apache.log4j.Logger;
import org.apache.log4j.xml.DOMConfigurator;

import se.exuvo.kartobotpc.comm.Devices;
import se.exuvo.kartobotpc.comm.KartoBot;
import se.exuvo.kartobotpc.comm.SerialConnection;
import se.exuvo.kartobotpc.comm.XbeeConnection;
import se.exuvo.kartobotpc.commands.Parser;
import se.exuvo.kartobotpc.gui.GUI;
import se.ihren.org.exportToLP.DataSet;
import se.ihren.org.exportToLP.ExportToLP;

import com.martiansoftware.jsap.FlaggedOption;
import com.martiansoftware.jsap.JSAP;
import com.martiansoftware.jsap.JSAPException;
import com.martiansoftware.jsap.JSAPResult;
import com.martiansoftware.jsap.Switch;

public class Init {
	protected static final Logger log = Logger.getLogger(Init.class);
	public static final long serialVersionUID = 1;;
	public static JSAPResult config = null;
	private static XbeeConnection xconn;
	private static SerialConnection sconn;
	public static ShutDownHook shutdownhook = new ShutDownHook();
	private static Input input;
	public static List<DataSet> sensors = new ArrayList<DataSet>();
	private static se.exuvo.kartobotpc.gui.GUI gui;
	
	static {
        //System.out.println("Checking if native libraries exist");
        //extractNatives();
    }

    private static void extractNatives() {
    	
		String lib = System.mapLibraryName("rxtxSerial");
		if(isWindows()){
			System.out.println("Detected OS: Windows");
			//lib="rxtxSerial.dll";
		}else if(isMac()){
			System.out.println("Detected OS: Mac");
			//lib="librxtxSerial.jnilib";
		}else if(isUnix()){
			System.out.println("Detected OS: Unix or Linux");
			//lib="librxtxSerial.so";
		}else{
			System.out.println("Unknown OS, Install rxtx native files for your os");
		}
		
		try {
			CommPortIdentifier.getPortIdentifier("");
        }catch (java.lang.UnsatisfiedLinkError e){
            System.out.println("Native RXTXcomm library not found");
            
            try {
                System.out.println("Extracting native library files, if necessary copy to java/lib directory for your OS");
                System.out.println("Short Install Instructions\n\nWindows\n\nrxtxSerial.dll goes in \\jre\\bin (under java)\n\nMac OS X (x86 and ppc) (there is an Installer with the source)\n\nlibrxtxSerial.jnilib goes in /Library/Java/Extensions\nGoogle RXTXcomm and install it.\n\nLinux (only x86, x86_64, ia64 here but more in the ToyBox)\n\nlibrxtxSerial.so goes in /jre/lib/[machine type] (i386 for instance) (under java)\nMake sure the user is in group lock or uucp so lockfiles work.\n\nSolaris (sparc only so far)\n\nlibrxtxSerial.so goes in /jre/lib/[machine type] (under java)\nMake sure the user is in group uucp so lockfiles work.");
                URL nativeurl = Thread.currentThread().getContextClassLoader().getResource(lib);
                InputStream is;
				is = nativeurl.openStream();
                OutputStream os = new FileOutputStream(lib);
                byte[] buf = new byte[4096];
                int cnt = is.read(buf);
                while (cnt > 0) {
                    os.write(buf, 0, cnt);
                    cnt = is.read(buf);
                }
                os.close();
                is.close();
            } catch (IOException e1) {
				e1.printStackTrace();
			}
        }catch(Throwable ignore){
        }
                
        
		try {
			CommPortIdentifier.getPortIdentifier("");
		}catch (java.lang.UnsatisfiedLinkError e){
			System.err.println("Error loading native RXTXcomm library, please install RXTXcomm library");
			System.exit(1);
		} catch (Throwable ignore) {
		}

    }
    
    private static boolean isWindows(){
		String os = System.getProperty("os.name").toLowerCase();
		//windows
	    return (os.indexOf( "win" ) >= 0); 
	}
 
	private static boolean isMac(){
		String os = System.getProperty("os.name").toLowerCase();
		//Mac
	    return (os.indexOf( "mac" ) >= 0); 
	}
 
	private static boolean isUnix(){
		String os = System.getProperty("os.name").toLowerCase();
		//linux or unix
	    return (os.indexOf( "nix") >=0 || os.indexOf( "nux") >=0);
	}

	public static void main(String[] args) {
		DOMConfigurator.configure("log4j.xml");
		log.fatal("### Starting ###");
		
		JSAP jsap = new JSAP();
        arguments(jsap);
        
        config = jsap.parse(args);
        // check whether the command line was valid, and if it wasn't, display usage information and exit.
        if (!config.success()) {
            System.err.println();
         // print out specific error messages describing the problems
            // with the command line, THEN print usage, THEN print full
            // help.  This is called "beating the user with a clue stick."
            for (Iterator<?> errs = config.getErrorMessageIterator();
                    errs.hasNext();) {
                System.err.println("Error: " + errs.next());
            }
            
            System.err.println();
            System.err.println("Usage: java "
                                + Init.class.getName());
            System.err.println("                "
                                + jsap.getUsage());
            System.err.println();
            // show full help as well
            System.err.println(jsap.getHelp());
            System.exit(1);
        }
        
        new Settings(config);
        Runtime.getRuntime().addShutdownHook(shutdownhook);
        Parser.init();
        xconn = new XbeeConnection();
        sconn = new SerialConnection();
        Devices.init();
        Devices.addDevice(new KartoBot());//TODO replace with automatic device detection
        
        if(config.getBoolean("test")){
        	net.java.games.input.test.ControllerReadTest.main(null);
        	return;
        }else if(config.getBoolean("list")){
        	net.java.games.input.test.ControllerTextTest.main(null);
        	return;
        }else{
        	if(Settings.getBol("xbeeAutoConnect")){
        		try {
        			getXConnection().open(Settings.getStr("xbeePort"), Settings.getInt("xbeeBaud"), Settings.getBol("xbeeFlowControl"));
    			} catch (Exception e) {
    				log.error("Failed to auto connect to xbee", e);
    			}
        	}
        }
        
        if(config.getBoolean("console")){
	    	new Thread(){
	    		public void run(){
	            	String a;
	            	Scanner s = new Scanner(System.in);
	        		while(true){
	        			System.out.print("$ ");
	        			a = s.nextLine();
	        			System.out.print(Parser.parse(a));
	        		}
	    		}
	    	}.start();
        }
        Init.getInput();
        gui = new GUI();
	}
	
	public static GUI getGUI(){
		return gui;
	}
	
	public static void logSensor(String name, float value){
		for(DataSet d : sensors){
			if(d.name.equals(name)){
				d.add(value);
				return;
			}
		}
		DataSet d = new DataSet(name);
		d.add(value);
		sensors.add(d);
	}
	
	public static void addSensor(String name, String shortName, String enhet){
		for(DataSet d : sensors){
			if(d.name.equals(name)){
				d.shortName = shortName;
				d.enhet = enhet;
				return;
			}
		}
		DataSet d = new DataSet(name);
		d.shortName = shortName;
		d.enhet = enhet;
		sensors.add(d);
	}
	
	public static XbeeConnection getXConnection(){
		return xconn;
	}
	
	public static SerialConnection getSConnection(){
		return sconn;
	}
	
	public static Input getInput(){
		if(input == null){
			input = new Input();
		}
		return input;
	}
	
	public static Input getInputHax(){
		return input;
	}
	
	private static final void arguments(JSAP jsap){
		
		Switch list = new Switch("list")
			.setShortFlag('l')
			.setLongFlag("list");
		list.setHelp("List controllers.");
		
		Switch test = new Switch("test")
			.setShortFlag('t')
			.setLongFlag("test");
		test.setHelp("Open test windows.");
		
		Switch console = new Switch("console")
		.setShortFlag('c')
		.setLongFlag("console");
		console.setHelp("Run in a console.");
		
		FlaggedOption port = new FlaggedOption("port")
			.setStringParser(JSAP.STRING_PARSER)
			.setDefault(JSAP.NO_DEFAULT)
			.setRequired(false)
			.setShortFlag('p')
			.setLongFlag("port");
		port.setHelp("Port to connect to XBee, overrides config.");
		
		try {
			jsap.registerParameter(list);
			jsap.registerParameter(console);
			jsap.registerParameter(test);
			jsap.registerParameter(port);
		} catch (JSAPException e) {
			log.warn("JSAP: Failed to register parameters due to: " + e);
		}
	}

}

class ShutDownHook extends Thread{
	
	public ShutDownHook(){
		
	}
	
	public void run(){
		try{
			Settings.save();
			if(Init.getInputHax() != null){
				Init.getInputHax().save();
			}
			if(Settings.getBol("logSensors")){
				Calendar c = Calendar.getInstance();
				ExportToLP.export(Init.sensors, new File("sensors-" +
						c.get(Calendar.YEAR) + "-" + c.get(Calendar.MONTH) + "-" +  c.get(Calendar.DAY_OF_MONTH) + "-" +  c.get(Calendar.HOUR_OF_DAY)
						+ ".cmbl"));
			}
			if(Init.getXConnection().isConnected()){
				Init.getXConnection().close();
			}
			if(Init.getSConnection().isConnected()){
				Init.getSConnection().close();
			}
		}catch(Throwable ignore){
			
		}
	}
}

