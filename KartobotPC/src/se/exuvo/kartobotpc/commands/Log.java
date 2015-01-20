package se.exuvo.kartobotpc.commands;

import org.apache.log4j.Level;

import se.exuvo.kartobotpc.Settings;
import se.exuvo.kartobotpc.logger;



public class Log extends Command {
	
	public Log(){
		super("log", "log [FATAL,ERROR,WARN,INFO,DEBUG,TRACE]");
	}

	@Override
	public int execute(String[] tokens, String phrase) {
		if(tokens.length > 1){
			Level l = Level.toLevel(tokens[1]);
			Settings.set("loglvl", l.toString());
			logger.reloadLogLvl();
			pl("Changed log level to " + l);
			return 0;
		}
		Level level = Level.toLevel(Settings.getStr("loglvl"));
		pl("Current log level " + level);
		return 0;
	}

}
