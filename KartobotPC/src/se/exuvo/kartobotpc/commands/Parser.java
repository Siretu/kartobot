package se.exuvo.kartobotpc.commands;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.log4j.Logger;

import se.exuvo.kartobotpc.ClassFinder;
import de.matthiasmann.twl.model.AutoCompletionDataSource;
import de.matthiasmann.twl.model.AutoCompletionResult;
import de.matthiasmann.twl.model.SimpleAutoCompletionResult;


public class Parser implements AutoCompletionDataSource{
	private static final Logger log = Logger.getLogger(Parser.class);
	private static List<Command> commands = new ArrayList<Command>();
	
	public static void init(){
		commands.clear();
		log.debug("Loading commands");
		try {
			List<Class<?>> l = ClassFinder.getClasses("se.exuvo.kartobotpc.commands");
			for(Class<?> c : l){
				if(Command.class.isAssignableFrom(c) && !c.equals(Command.class)){
					try {
						Class<? extends Command> cc = c.asSubclass(Command.class);
						Command p = cc.newInstance();
						if(p.getName() != null && !p.getName().equals("")){
							commands.add(p);
							log.trace("Loaded command: " + p.getName());
						}
					} catch (Throwable e) {
						log.warn("Failed to load command: \"" + c.getSimpleName() + "\"", e);
					} 
				}
			}
		} catch (ClassNotFoundException e) {
			log.warn("Failed to load commands", e);
		} catch (IOException e) {
			log.warn("Failed to load commands", e);
		}
	}
	
	public static int parse(String phrase){
		phrase = phrase.trim();
		log.debug("Parsing Command:" + phrase);
		String delims = "[ ]+";
		String[] tokens = phrase.split(delims);
		int ret = parsecommand(tokens, phrase);
		return  ret;
	}
	
	private static int parsecommand(String[] tokens, String phrase){
		for(int i=0;i<tokens.length;i++){
			String t = tokens[i];
			if(t.length() > 1 && t.charAt(0) == '$'){
				t = Command.getVar(t.substring(1));
				phrase = phrase.replace(tokens[i], t);
				tokens[i] = t;
			}
		}
		if(tokens[0].contains("=")){
				String name = tokens[0].substring(0, tokens[0].indexOf("="));
				String value = phrase.substring(phrase.indexOf("=")+1, phrase.length());
				Command.setVar(name, value);
				Command.out.print(name + "=" + value);
				return 0;
		}

		
		if(tokens[0].equalsIgnoreCase("help")){
			if(tokens.length > 1){
				for(Iterator<Command> it = commands.iterator();it.hasNext();){
					Command c = it.next();
					for(Iterator<String> names = c.getNames().iterator();names.hasNext();){
						if(tokens[1].equalsIgnoreCase(names.next())){
							c.help();
						}
					}
				}
			}else{
				for(Iterator<Command> it = commands.iterator();it.hasNext();){
					Command c = it.next();
					c.help();
				}
			}
			return 0;
		}else{
			for(Iterator<Command> it = commands.iterator(); it.hasNext();){
				Command c = it.next();
				for(Iterator<String> names = c.getNames().iterator();names.hasNext();){
					if(tokens[0].equalsIgnoreCase(names.next())){
						try{
							return c.execute(tokens, phrase);
						}catch(Throwable t){
							t.printStackTrace();
							Command.out.println("Command failed: " + t);
							return -1;
						}
					}
				}
			}
		}
		
		Command.out.println("Unknown command \nTo see a list of avaible commands enter \"help\"");
		return -2;
	}

	@Override
	public AutoCompletionResult collectSuggestions(String text, int cursorPos, AutoCompletionResult prev) {
		text = text.substring(0, cursorPos);
		int prefixLength = text.indexOf(" ");
        if(prefixLength < 0) {
            prefixLength = text.length();//0
        }
        String prefix = text.substring(0, prefixLength);
		
		List<String> results = new ArrayList<String>();
		parse("echo \nAutoCompleting \"" + prefix + "\" " + prefixLength);
		
//        if((prev instanceof SimpleAutoCompletionResult) && prev.getPrefixLength() == prefixLength &&
//        	prev.getText().startsWith(prefix)) {
//			for(int i=0; i < prev.getNumResults(); i++){
//				String name = prev.getResult(i);
//				if(name.startsWith(text)){
//					results.add(name);
//					parse("echo \"" + name + "\", ");
//				}
//			}
//        }elseO{
//      	}
		if(text.length() == prefixLength){
			for(Command c : commands){
    			for(String name : c.getNames()){
    				if(name.startsWith(text)){
    					results.add(name);
    					parse("echo \"" + name + "\", ");
    				}
    			}
    		}
		}else{
			//TODO auto-complete sub command
		}
        	
		if(results.size() == 0) {
            return null;
        }
		
		return new SimpleAutoCompletionResult(text, prefixLength, results);
	}
}

