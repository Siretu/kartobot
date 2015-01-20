package se.exuvo.kartobotpc.commands;

import se.exuvo.kartobotpc.Init;


public class Version extends Command {
	
	public Version(){
		super("version", "");
	}

	@Override
	public int execute(String[] tokens, String phrase) {
		pl("Current version is " + Init.serialVersionUID);
		return 0;
	}

}
