package se.exuvo.kartobotpc.commands;

import java.io.InputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.log4j.Logger;

public abstract class Command {
	public abstract int execute(String[] tokens, String phrase);
	
	protected static Logger log = Logger.getLogger(Command.class);
	protected static PrintStream out;
	private static HashMap<String, String> vars = new HashMap<String, String>();
	protected static InputStream in;
	
	private List<String> names = new ArrayList<String>();
	private String name,description;
	
	public Command(String name, String description){
		this.name = name;
		this.description = description;
		names.add(name);
		out = System.out;
		in = System.in;
	}
	
	public String getName(){
		return name;
	}
	
	public List<String> getNames(){
		return names;
	}
	
	public String getDescription(){
		return description;
	}
	
	protected void addName(String name){
		names.add(name);
	}
	
	public static void setOut(PrintStream o){
		out = o;
	}
	
	public static void setIn(InputStream i){
		in = i;
	}
	
	public static PrintStream getOut(){
		return out;
	}
	
	public static InputStream getIn(){
		return in;
	}
	
	protected void p(String s){
		out.print(s);
	}
	
	protected void pl(String s){
		out.println(s);
	}
	
	public static void setVar(String key, String value){
		vars.put(key, value);
	}
	
	public static String getVar(String key){
		String v = vars.get(key);
		return v == null ? "" : v; 
	}
	
	public void help(){
		pl(getName() + ":\n  " + getDescription());
	}

}
