package se.exuvo.kartobotpc.commands;

import se.exuvo.kartobotpc.Init;

public class Quit extends Command {
	
	public Quit(){
		super("quit", "");
		addName("exit");
	}

	@Override
	public int execute(String[] tokens, String phrase) {
		log.fatal("### Exiting ###");
		if(Init.getXConnection() != null)
			Init.getXConnection().close();
		System.exit(0);
		return 0;
	}

}
