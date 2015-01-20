package se.exuvo.kartobotpc;

import com.martiansoftware.jsap.JSAPResult;

public class Settings extends se.exuvo.settings.Settings{
	
	public static void clean(){
		//remove("xbeeRSSI");
	}
	
	public static boolean save(){
		clean();
		return se.exuvo.settings.Settings.save();
	}
	
	public Settings(JSAPResult conf){
		add("loglvl", "INFO");
		add("logSensors", false);
		add("xbeePort", "COM12");
		add("xbeeAutoConnect", false);
		add("xbeeBaud", 115200);
		add("xbeeFlowControl", true);
		add("xbeeLogRSSI", true);
		add("remoteAddress64", "");
		add("GUI.FrameLimit", 30);
		add("GUI.Width", 800);
		add("GUI.Height", 600);
		add("GUI.Fullscreen", false);
		add("GUI.VSync", false);
		add("GUI.AlwaysRender", true);
		add("GUI.ShowFPS", true);
		add("SoundVolume", 1f);
		add("MusicVolume", 0.8f);
		
		if(!start(conf,"kartobot")){
			log.fatal("Failed to read settings from file, please fix. Exiting.");
			System.exit(1);
		}
		
		logger.reloadLogLvl();
	}
	

	
}
