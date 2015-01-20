package se.exuvo.kartobotpc.commands;

import se.exuvo.kartobotpc.Init;
import se.exuvo.kartobotpc.comm.Devices;
import se.exuvo.kartobotpc.comm.KartoBot;
import se.exuvo.kartobotpc.gui.GUI;
import se.exuvo.kartobotpc.gui.Main;



public class Clear extends Command {
	
	public Clear(){
		super("clear", "clear [local, remote, pings, map, path, tasks]");
	}

	@Override
	public int execute(String[] tokens, String phrase) {
		if(tokens.length > 1){
			if("local".equals(tokens[1])){
				((Main)Init.getGUI().getState(GUI.MAINSTATE)).consoleClear();
			}else if("remote".equals(tokens[1])){
				((Main)Init.getGUI().getState(GUI.MAINSTATE)).console2Clear();
			}else if("pings".equals(tokens[1])){
				((Main)Init.getGUI().getState(GUI.MAINSTATE)).clearSonarPings();
			}else if("map".equals(tokens[1])){
				((Main)Init.getGUI().getState(GUI.MAINSTATE)).clearMap();
			}else if("path".equals(tokens[1])){
				((Main)Init.getGUI().getState(GUI.MAINSTATE)).clearPath();
			}else if("tasks".equals(tokens[1])){
				synchronized(((KartoBot)Devices.getDevice("Kartobot")).tasks){
					((KartoBot)Devices.getDevice("Kartobot")).tasks.clear();
				}
			}
		}else{
			((Main)Init.getGUI().getState(GUI.MAINSTATE)).clearSonarPings();
			((Main)Init.getGUI().getState(GUI.MAINSTATE)).clearPath();
			((Main)Init.getGUI().getState(GUI.MAINSTATE)).clearMap();
		}
		
		return 0;
	}
}
