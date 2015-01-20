package se.exuvo.kartobotpc.commands;

import se.exuvo.kartobotpc.Init;
import se.exuvo.kartobotpc.gui.GUI;
import se.exuvo.kartobotpc.gui.Main;

public class Zoom extends Command {

	public Zoom() {
		super("zoom", "Change map zoom level");
		
	}

	@Override
	public int execute(String[] tokens, String phrase) {
		if(tokens.length > 1){
			((Main)Init.getGUI().getState(GUI.MAINSTATE)).setZoomLevel(new Float(tokens[1]));
			return 0;
		}
		
		help();
		return 0;
	}
}
