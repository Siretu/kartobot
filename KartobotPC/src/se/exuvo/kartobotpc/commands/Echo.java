package se.exuvo.kartobotpc.commands;



public class Echo extends Command {
	
	public Echo(){
		super("echo", "echo text");
	}

	@Override
	public int execute(String[] tokens, String phrase) {
		p(phrase.substring(tokens[0].length()));
		return 0;
	}

}
