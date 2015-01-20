package se.exuvo.kartobotpc.gui;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.atomic.AtomicBoolean;

import org.apache.log4j.Logger;
import org.newdawn.slick.CachedRender;
import org.newdawn.slick.Color;
import org.newdawn.slick.GameContainer;
import org.newdawn.slick.Graphics;
import org.newdawn.slick.Image;
import org.newdawn.slick.Input;
import org.newdawn.slick.SlickException;
import org.newdawn.slick.Sound;
import org.newdawn.slick.particles.ParticleSystem;
import org.newdawn.slick.state.StateBasedGame;

import se.exuvo.kartobotpc.Init;
import se.exuvo.kartobotpc.ResourceManager;
import se.exuvo.kartobotpc.comm.Device;
import se.exuvo.kartobotpc.comm.Devices;
import se.exuvo.kartobotpc.comm.KartoBot;
import se.exuvo.kartobotpc.comm.KartoBot.PID;
import se.exuvo.kartobotpc.comm.KartoBot.SonarPing;
import se.exuvo.kartobotpc.comm.KartoBot.Square;
import se.exuvo.kartobotpc.comm.KartoBot.Task;
import se.exuvo.kartobotpc.commands.Command;
import se.exuvo.kartobotpc.commands.Parser;
import TWLSlick.BasicTWLGameState;
import de.matthiasmann.twl.Button;
import de.matthiasmann.twl.DraggableButton;
import de.matthiasmann.twl.DraggableButton.DragListener;
import de.matthiasmann.twl.EditField;
import de.matthiasmann.twl.EditField.Callback;
import de.matthiasmann.twl.Label;
import de.matthiasmann.twl.PopupWindow;
import de.matthiasmann.twl.ScrollPane;
import de.matthiasmann.twl.TextArea;
import de.matthiasmann.twl.ToggleButton;
import de.matthiasmann.twl.Widget;
import de.matthiasmann.twl.model.OptionBooleanModel;
import de.matthiasmann.twl.model.SimpleIntegerModel;
import de.matthiasmann.twl.textarea.SimpleTextAreaModel;
import de.matthiasmann.twl.textarea.TextAreaModel;
import de.matthiasmann.twl.textarea.TextAreaModel.TextElement;

public class Main extends BasicTWLGameState {
	private static final Logger log = Logger.getLogger(Main.class);
	
	private Input input;
	private static ScheduledExecutorService executor = Executors.newSingleThreadScheduledExecutor();
	private ParticleSystem system;
	
	private float screenX, screenY;
	private float zoom = 2;
	private final float ZOOM_CHANGE_MULT = 0.02f;
	
	private int oldSelected;
	private Button options, controllers;
	private ToggleButton mapTabB, statusTabB, settingsTabB;
	
	private final int MAP_TAB = 0, STATUS_TAB = 1, SETTINGS_TAB = 2;
	private int selectedTabInd = MAP_TAB;
	private SimpleIntegerModel tabModel;
	
	private final int GRID_SQUARE_SIZE = 10;
	private final int GRID_MAX_SIZE = 64;
	
	private Image imgKartobot, imgKartobotMap;
	
	private boolean consoleLock = false;
	private Widget console;
	private Label consoleLabel;
	private EditField consoleField;
	private List<String> consoleFieldOld = new ArrayList<String>();
	private int consoleFieldOldPos = -1;
	private TextArea consoleArea;
	private ScrollPane consoleScrollPane;
	private SimpleTextAreaModel consoleModel;
	private ConsoleBuffer consoleBufferIn = new ConsoleBuffer();
	private StringBuffer consoleBufferOut = new StringBuffer(32);
	
	private boolean console2Lock = false;
	private Widget console2;
	private Label console2Label;
	private EditField console2Field;
	private List<String> console2FieldOld = new ArrayList<String>();
	private int console2FieldOldPos = -1;
	private TextArea console2Area;
	private ScrollPane console2ScrollPane;
	private SimpleTextAreaModel console2Model;
	
	private DraggableButton consoleHeight;
	
	public static Sound error;
	public static Sound select;
	public static Sound pick;
	
	private CachedRender cachedGrid, cachedMap, cachedPath;
	private boolean dirtyGrid = true;
	
	private KartoBot kartobot;
	
	private final int SONAR_PING_LIFE_MS = 3500;
	private final int MAX_SONAR_PINGS = 150;
	private List<SonarPing> sonarPings = new ArrayList<SonarPing>();
	
	private Widget commandsWindow;
	private Widget PIDsWindow;
	public AtomicBoolean reloadPids = new AtomicBoolean();
	private EditField speedField;
	
	private B rmbCommand;
	private SimpleIntegerModel rmbModel;
	
	public Main(){
	}

	@Override
	public int getID() {
		return GUI.MAINSTATE;
	}
	
	@Override
	public void enter(GameContainer container, StateBasedGame game) throws SlickException {
		super.enter(container,game);
		log.info("Entered Main state");
		
		screenX=0;
		screenY=0;
		consoleField.setText("");
		console2Field.setText("");
		consoleModel.setText("");
		console2Model.setText("");
		GUI.getBackgroundMusic().stop();
		
		if(console.isVisible()){
			console.requestKeyboardFocus();
		}
	}
	
	@Override
	public void initResources(){
		system = new ParticleSystem("org/newdawn/slick/data/particle.tga", 2000);
		error = ResourceManager.getSound("sounds/Starcraft/Misc/BUZZ.WAV");
		select = ResourceManager.getSound("sounds/Megaman X3/SE/SE_1E.SE.wav");//1e,1C
		pick = ResourceManager.getSound("sounds/Megaman X3/SE/SE_1C.SE.wav");//5A,,1D
		imgKartobot = ResourceManager.getImage("images/chassi.png");
		imgKartobotMap = ResourceManager.getImage("images/chassi-map.png").getScaledCopyKeepAspectRatio(23, 25);//22.5, 24.5
	}

	@Override
	public void init(final GameContainer container, StateBasedGame game) throws SlickException {
		log.info("Initializing Main state");
		
		input = container.getInput();
		kartobot = ((KartoBot)Devices.getDevice("Kartobot"));
		
		Command.setOut(new PrintStream(new OutputStream() {
			@Override
			public void write(int b) throws IOException {
				synchronized(consoleBufferOut){
					consoleBufferOut.append((char) b);
				}
			}
		}, true));
		Command.setIn(new InputStream() {
			@Override
			public int read() throws IOException {
				return consoleBufferIn.read();
			}
		});
	}
	
	@Override
	public void render(final GameContainer container, StateBasedGame game, final Graphics g) throws SlickException {
		g.setAntiAlias(true);
		synchronized (consoleModel){
			if(consoleLock){
				consoleScrollPane.setScrollPositionY(consoleScrollPane.getMaxScrollPosY());
				consoleLock = false;
			}
		}
		synchronized (console2Model){
			if(console2Lock){
				console2ScrollPane.setScrollPositionY(console2ScrollPane.getMaxScrollPosY());
				console2Lock = false;
			}
		}
		
		
		switch(selectedTabInd) {
			case MAP_TAB:{
				g.translate(-screenX, -screenY);
				g.scale(zoom, zoom);
				
				if(dirtyGrid){
					cachedGrid = new CachedRender(new Runnable() {
						@Override
						public void run() {
							g.setColor(Color.gray);
							for(int x =  0; x < GRID_MAX_SIZE+1; x++) {
									g.drawLine(x * GRID_SQUARE_SIZE, 0, x * GRID_SQUARE_SIZE, GRID_SQUARE_SIZE * GRID_MAX_SIZE);
							}
							for(int y =  0; y < GRID_MAX_SIZE+1; y++) {
								g.drawLine(0, y * GRID_SQUARE_SIZE, GRID_SQUARE_SIZE * GRID_MAX_SIZE, y * GRID_SQUARE_SIZE);
							}
						}
					});
					dirtyGrid = false;
				}
				cachedGrid.render();
				
				if(kartobot.dirtyMap){
					cachedMap = new CachedRender(new Runnable() {
						@Override
						public void run() {
							synchronized(kartobot.map) {
								for(int x = 0; x < GRID_MAX_SIZE; x++) {
									for(int y =  0; y < GRID_MAX_SIZE; y++) {
										int c = Math.min(255, kartobot.map[x][y]*10);
										if(c > 0){
											g.setColor(new Color(c , c/2, c/2));
											g.fillRect(x * GRID_SQUARE_SIZE + 1, y * GRID_SQUARE_SIZE + 1, GRID_SQUARE_SIZE - 2, GRID_SQUARE_SIZE - 2);
										}
									}
								}
							}
						}
					});
					kartobot.dirtyMap = false;
				}
				cachedMap.render();
				
				synchronized(sonarPings) {
					if(sonarPings.size() > MAX_SONAR_PINGS) {
						for(int a = MAX_SONAR_PINGS; a < sonarPings.size(); a++) {
							sonarPings.remove(0);
						}
					}
					for(Iterator<SonarPing> iter = sonarPings.iterator(); iter.hasNext();) {
						SonarPing ping = iter.next();
						long deltaTime = System.currentTimeMillis() - ping.time;
						float ratio = (float) deltaTime / SONAR_PING_LIFE_MS;
						if(ratio > 1) {
							ratio = 1;
						}
						g.setColor(new Color(1f, 1f, 0f, 1 - 0.65f * ratio));
						float diameter = 0.8f * GRID_SQUARE_SIZE * (1 - 0.8f * ratio);
						g.fillOval(ping.x - diameter / 2, ping.y - diameter / 2, diameter, diameter);
					}
				}
				
				if(kartobot.dirtyPath){
					cachedPath = new CachedRender(new Runnable() {
						@Override
						public void run() {
							int o = GRID_SQUARE_SIZE;
							synchronized(kartobot.path) {
								g.setColor(new Color(0, 200, 0));
								for(int i=0; i<kartobot.path.size(); i++){
									Square s = kartobot.path.get(i);
									g.drawLine(s.x*o + 1, s.y*o + 1, s.x*o + o - 1, s.y*o + o - 1);
									g.drawLine(s.x*o + o - 1, s.y*o + 1, s.x*o + 1, s.y*o + o - 1);
								}
								
//								float f = 3f;
//								g.scale(1/f, 1/f);
//								g.setColor(new Color(80, 200, 80));
//								for(int i=0; i<kartobot.path.size(); i++){
//									Square s = kartobot.path.get(i);
//									if(i == 0){
//										g.drawString("", 0, 0);//Fix for first drawString does not work
//									}
//									
//									g.drawString(""+(i+1), f*(s.x*o + o/2 - g.getFont().getWidth(""+(i+1))/(2*f)), f*(s.y*o + o/2));
//									
//									if(i == kartobot.path.size()-1){
//										g.drawString("", 0, 0);//Fix for last drawString does not always work
//									}
//								}
//								g.scale(f, f);
							}
						}
					});
					kartobot.dirtyPath = false;
				}
				cachedPath.render();
				synchronized(kartobot.path) {
					int o = GRID_SQUARE_SIZE;
					float f = 3f;
					
					g.scale(1/f, 1/f);
					g.setColor(new Color(80, 200, 80));
					for(int i=0; i<kartobot.path.size(); i++){
						Square s = kartobot.path.get(i);
						g.drawString(""+(i+1), f*(s.x*o + o/2 - g.getFont().getWidth(""+(i+1))/(2*f)), f*(s.y*o + o/2));
					}
					g.scale(f, f);
				}
				
				double yaw = (Math.PI/2 - Math.toRadians(kartobot.yaw));
				
				imgKartobotMap.rotate(kartobot.yaw - imgKartobotMap.getRotation());
				imgKartobotMap.draw(kartobot.x - imgKartobotMap.getWidth() / 2 + 0.5f*(float)Math.cos(yaw) + 0.5f*(float)Math.cos(yaw + Math.PI/2),
						kartobot.y - imgKartobotMap.getHeight() / 2 - 0.5f*(float)Math.sin(yaw) - 0.5f*(float)Math.sin(yaw + Math.PI/2), Color.blue);
				
				g.setColor(Color.green);
				int xc = kartobot.x;
				int yc = kartobot.y;
				float s = imgKartobotMap.getWidth() / 2;
				g.drawLine((float) (xc + s/2*Math.cos(yaw)), (float) (yc - s/2*Math.sin(yaw)), (float) (xc + s*Math.cos(yaw)), (float) (yc - s*Math.sin(yaw)));
				
				g.resetTransform();
				break;
			}
			case STATUS_TAB:{
				{
					int width = imgKartobot.getWidth();
					int height = imgKartobot.getHeight();
					int x = Math.max((container.getWidth() - width) / 2, 250);
					int y = Math.max((container.getHeight() - height) / 4, 40);
					imgKartobot.draw(x, y);
					
					int xc = x + 220;
					int yc = y + 120;
					
					g.setColor(Color.white);
					g.drawString("Throttle: " + kartobot.throttleL, x + width/2 - 225, y + 30 - g.getFont().getLineHeight());
					g.drawString("Speed: " + kartobot.speedL, x + width/2 - 225, y + 30);
					
					g.drawString("Throttle: " + kartobot.throttleR, x + width/2 + 110, y + 30 - g.getFont().getLineHeight());
					g.drawString("Speed: " + kartobot.speedR, x + width/2 + 110, y + 30);
				
					g.drawString("X: " + kartobot.x, xc, yc + 1 * g.getFont().getLineHeight());
					g.drawString("Y: " + kartobot.y, xc, yc + 2 * g.getFont().getLineHeight());
					g.drawString("Heading: " + kartobot.heading, xc, yc + 3 * g.getFont().getLineHeight());
					g.drawString("Roll: " + kartobot.roll + "°", xc, yc + 4 * g.getFont().getLineHeight());
					g.drawString("Pitch: " + kartobot.pitch + "°", xc, yc + 5 * g.getFont().getLineHeight());
					g.drawString("Yaw " + kartobot.yaw + "°", xc, yc + 6 * g.getFont().getLineHeight());
					g.drawString("SetHeading " + kartobot.setHeading + "°", xc, yc + 7 * g.getFont().getLineHeight());
					
					g.drawString("Order: " + kartobot.order, xc, yc + 9 * g.getFont().getLineHeight());
					
					g.drawString("U: " + kartobot.batteryU + "V", xc, yc + 14 * g.getFont().getLineHeight());
					g.drawString("I: " + kartobot.batteryI + "A", xc, yc + 15 * g.getFont().getLineHeight());
					g.drawString("Source: " + kartobot.powerSource, xc, yc + 16 * g.getFont().getLineHeight());
				}
				
				synchronized(kartobot.tasks){
					int x = 10;
					g.drawString("#  Name            Time  %   T%", x, 30 + 1 * g.getFont().getLineHeight());
					for(int i=0; i< kartobot.tasks.size(); i++){
						Task t = kartobot.tasks.get(i);
						StringBuffer s = new StringBuffer(32);
						s.append(t.id + " ");
						for(int u=2 - Integer.toString(t.id).length(); u>0; u--){
							s.append(' ');
						}
						s.append(t.name + " ");
						for(int u=15 - t.name.length(); u>0; u--){
							s.append(' ');
						}
						s.append(t.time + " ");
						for(int u=5 - Integer.toString(t.time).length(); u>0; u--){
							s.append(' ');
						}
						s.append(t.P + " ");
						for(int u=3 - Integer.toString(t.P).length(); u>0; u--){
							s.append(' ');
						}
						s.append(t.totalP);
						for(int u=3 - Integer.toString(t.totalP).length(); u>0; u--){
							s.append(' ');
						}
						g.drawString(s.toString(), x, 30 + (2+i) * g.getFont().getLineHeight());
					}
				}
				
				//TODO add buttons for commands
				
				break;
			}
			case SETTINGS_TAB:{
				synchronized(reloadPids){
					if(reloadPids.compareAndSet(true, false)){
						reloadPIDs();
					}
				}
				break;
			}
		}
		
		g.setColor(Color.white);
		g.drawString("In: " + Init.getXConnection().speedIn, 100, 10);
		g.drawString("Out: " + Init.getXConnection().speedOut, 190, 10);
		g.drawString("Pathing: " + kartobot.pathLength +  " " + kartobot.pathTime + "ms", 280, 10);
		
		if(Init.getSConnection().isConnected()){
			g.drawString("Serial In: " + Init.getSConnection().speedIn, 37, 28);
			g.drawString("Out: " + Init.getSConnection().speedOut, 190, 28);
		}
		
		system.render();
	}

	@Override
	public void update(GameContainer container, final StateBasedGame game, int delta) throws SlickException {
		float d = delta/1000.0f;
			if(!console.hasKeyboardFocus() && !console2.hasKeyboardFocus()){
				float s = 200*d;
		    	if(input.isKeyDown(Input.KEY_UP)){
		    		screenY-=s;
		    	}else if(input.isKeyDown(Input.KEY_DOWN)){
		    		screenY+=s;
		    	}
		    	if(input.isKeyDown(Input.KEY_LEFT)){
		    		screenX-=s;
		    	}else if(input.isKeyDown(Input.KEY_RIGHT)){
		    		screenX+=s;
		    	}
			}
			if(consoleBufferIn.hasLine()){
				executor.submit(new Runnable() {
					@Override
					public void run() {
						String text = consoleBufferIn.readLine();
						synchronized(consoleBufferOut){
							consoleBufferOut.append("$ " + text);
						}
						if(Parser.parse(text) != 0){
							error.play();
						}
					}
				});
			}
			if(consoleBufferOut.length() > 0){
				synchronized(consoleBufferOut){
					consoleAppend(consoleBufferOut.toString());
					consoleBufferOut.delete(0, consoleBufferOut.length());
				}
			}
			
			int selected = 0;
			int mouseX = container.getInput().getMouseX();
			int mouseY = container.getInput().getMouseY();
			
			if(options.isInside(mouseX, mouseY))selected = 1;
			if(controllers.isInside(mouseX, mouseY))selected = 2;
			if(mapTabB.isInside(mouseX, mouseY))selected = 3;
			if(statusTabB.isInside(mouseX, mouseY))selected = 4;
			
			if(selected != oldSelected && selected > 0){
				Main.select.play();
			}
			oldSelected = selected;
		
			system.update(delta);
			
			if(! kartobot.sonar.isEmpty()){
				synchronized(sonarPings){
					sonarPings.add(kartobot.sonar.poll());
				}
			}
	}
	
	public void clearSonarPings() {
		synchronized(sonarPings) {
			sonarPings.clear();
		}
	}
	
	public void clearPath() {
		synchronized(kartobot.path) {
			kartobot.path.clear();
			kartobot.dirtyPath = true;
		}
	}
	
	public void clearMap() {
		synchronized(kartobot.map) {
			for(int x=0; x<128; x++){
				for(int y=0; y<128; y++){
					kartobot.map[x][y] = 0;
				}
			}
			kartobot.dirtyMap = true;
		}
	}
	
	public void setZoomLevel(float f){
		zoom = f;
	}
	
	private void consoleAppend(String s){
		synchronized (consoleModel) {
			consoleLock = consoleLock || consoleScrollPane.getScrollPositionY() == consoleScrollPane.getMaxScrollPosY();
			
			TextAreaModel.TextElement t = (TextElement)consoleModel.iterator().next();
			String a = t.getText() + s;
			if(a.length() > 9000){
				a = a.substring(1000, a.length());
			}
			consoleModel.setText(a);
		}
	}
	
	public void console2Append(String s){
		synchronized (console2Model) {
			console2Lock = console2Lock || console2ScrollPane.getScrollPositionY() == console2ScrollPane.getMaxScrollPosY();
			
			TextAreaModel.TextElement t = (TextElement)console2Model.iterator().next();
			String a = t.getText() + s;
			if(a.length() > 9000){
				a = a.substring(1000, a.length());
			}
			console2Model.setText(a);
		}
	}
	
	public void consoleClear(){
		synchronized (consoleModel){
			consoleLock = consoleLock || consoleScrollPane.getScrollPositionY() == consoleScrollPane.getMaxScrollPosY();
			consoleModel.setText("");
		}
	}
	
	public void console2Clear(){
		synchronized (console2Model){
			console2Lock = console2Lock || console2ScrollPane.getScrollPositionY() == console2ScrollPane.getMaxScrollPosY();
			console2Model.setText("");
		}
	}
	
	class B{
    	String name, desc, command, useDesc;
    	public B(String n, String d, String c, String uD){
    		name = n;
    		desc = d;
    		command = c;
    		useDesc = uD;
    	}
    }
	
	private void Parse(String phrase){
		if(Parser.parse(phrase) != 0){
			error.play();
		}
	}
	
	public void reloadPIDs(){
		PIDsWindow.removeAllChildren();
		{
	    	Label l = new Label("Name             ");
	    	PIDsWindow.add(l);
	    	
	    	l = new Label("P             ");
	    	PIDsWindow.add(l);
	    	
	    	l = new Label("I             ");
	    	PIDsWindow.add(l);
	    	
	    	l = new Label("D             ");
	    	PIDsWindow.add(l);
	    	
	    	Button save = new Button("S");
	    	save.setTooltipContent("Save All");
	    	save.addCallback(new Runnable() {
				@Override
				public void run() {
					synchronized(kartobot.PIDs){
						int[] d = new int[kartobot.PIDs.length * 3];
						int z=0;
						
						for(int i=0; i < kartobot.PIDs.length; i++){
							final PID p = kartobot.PIDs[i];
							if(p != null){
								d[z++] = 'P';
								d[z++] = 'S';
								d[z++] = p.id;
							}
						}
						
						kartobot.getC().send(Arrays.copyOf(d, z));
					}
				}
			});
	    	PIDsWindow.add(save);
	    	
	    	Button load = new Button("L");
	    	load.setTooltipContent("Load All");
	    	load.addCallback(new Runnable() {
				@Override
				public void run() {
					int[] d = new int[kartobot.PIDs.length * 6];
					int z=0;
					
					for(int i=0; i < kartobot.PIDs.length; i++){
						final PID p = kartobot.PIDs[i];
						if(p != null){
							d[z++] = 'P';
							d[z++] = 'L';
							d[z++] = p.id;
							d[z++] = 'P';
							d[z++] = 'g';
							d[z++] = p.id;
						}
					}
					
					kartobot.getC().send(Arrays.copyOf(d, z));
				}
			});
	    	PIDsWindow.add(load);
	    }
		synchronized(kartobot.PIDs){
			for(int i=0; i < kartobot.PIDs.length; i++){
				final PID p = kartobot.PIDs[i];
				if(p != null){
					Label l = new Label(p.name);
			    	PIDsWindow.add(l);
			    	
			    	final EditField fp = new EditField();
			    	final EditField fi = new EditField();
			    	final EditField fd = new EditField();
			    	
			    	fp.setText("" + p.P);
			    	fi.setText("" + p.I);
			    	fd.setText("" + p.D);
			    	
			    	fp.setForwardUnhandledKeysToCallback(true);
			    	fi.setForwardUnhandledKeysToCallback(true);
			    	fd.setForwardUnhandledKeysToCallback(true);
			    	
			    	Callback c = new Callback() {
						@Override
						public void callback(int key) {
							if(key == Input.KEY_ENTER || key == Input.KEY_NUMPADENTER){
								pick.play();
								try{
									float P = Float.parseFloat(fp.getText());
									float I = Float.parseFloat(fi.getText());
									float D = Float.parseFloat(fd.getText());
									
									int[] d = Device.writeFloat(D, Device.writeFloat(I, Device.writeFloat(P, new int[]{'P', 's', p.id})));
									kartobot.getC().send(d);
									
									synchronized(kartobot.PIDs){
										kartobot.PIDs[p.id].P = P;
										kartobot.PIDs[p.id].I = I;
										kartobot.PIDs[p.id].D = D;
									}
								}catch(NumberFormatException e){
									disp("Please use floats: " + e.getMessage());
								}
							}
						}
					};
			    	
			    	fp.addCallback(c);
			    	fi.addCallback(c);
			    	fd.addCallback(c);
			    	
			    	Button save = new Button("S");
			    	save.setTooltipContent("Save");
			    	save.addCallback(new Runnable() {
						@Override
						public void run() {
							kartobot.getC().send(new int[]{'P', 'S', p.id});
						}
					});
			    	
			    	Button load = new Button("L");
			    	load.setTooltipContent("Load");
			    	load.addCallback(new Runnable() {
						@Override
						public void run() {
							kartobot.getC().send(new int[]{'P', 'L', p.id, 'P', 'g', p.id});
						}
					});
			    	
			    	PIDsWindow.add(fp);
			    	PIDsWindow.add(fi);
			    	PIDsWindow.add(fd);
			    	PIDsWindow.add(save);
			    	PIDsWindow.add(load);
				}
			}
		}
//		rootPane.invalidateLayout();
		layoutPIDWindow();
	}
	
	private void loadCommands(int tab){
		commandsWindow.setVisible(false);
		commandsWindow.removeAllChildren();
		commandsWindow.invalidateLayout();
		
		class A{
	    	String name, desc, command;
	    	public A(String n, String d, String c){
	    		name = n;
	    		desc = d;
	    		command = c;
	    	}
	    }
	    
	    List<A> a = new ArrayList<A>();
		
		if(tab == MAP_TAB){
			List<B> d = new ArrayList<B>();
			
			d.add(new B("Nav Tar", "Set Nav Target to mouse", "send ng", "Nav target:"));
			d.add(new B("Pos", "Set KartoBot position", "send mG", "Position:"));
			d.add(new B("Wall", "Add a wall", "send nw", "Adding wall:"));
			
			if(rmbModel == null){
				rmbModel = new SimpleIntegerModel(0, 2, 0);
				rmbCommand = d.get(rmbModel.getValue());
			}
			
			int i=0;
			for(final B c : d){
				final ToggleButton b = new ToggleButton(new OptionBooleanModel(rmbModel, i++));
				b.setText(c.name);
				b.setTooltipContent(c.desc);
				b.addCallback(new Runnable() {
					@Override
					public void run() {
						rmbCommand = c;
					}
				});
				commandsWindow.add(b);
			}
			
			{
				final ToggleButton b = new ToggleButton("XBee");
				b.setTooltipContent("");
				b.setActive(Init.getXConnection().isConnected());
				Init.getXConnection().addConnectCallback(new Runnable() {
					@Override
					public void run() {
						b.setActive(Init.getXConnection().isConnected());
					}
				});
				b.addCallback(new Runnable() {
					@Override
					public void run() {
						executor.submit(new Runnable() {
							@Override
							public void run() {
								if(Init.getXConnection().isConnected()){
									Parse("xbee disconnect");
								}else{
									Parse("xbee connect");
								}
								b.setActive(Init.getXConnection().isConnected());
							}
						});
					}
				});
				commandsWindow.add(b);
			}
			{
				final ToggleButton b = new ToggleButton("Serial");
				b.setTooltipContent("");
				b.setActive(Init.getSConnection().isConnected());
				Init.getSConnection().addConnectCallback(new Runnable() {
					@Override
					public void run() {
						b.setActive(Init.getSConnection().isConnected());
					}
				});
				b.addCallback(new Runnable() {
					@Override
					public void run() {
						executor.submit(new Runnable() {
							@Override
							public void run() {
								if(Init.getSConnection().isConnected()){
									Parse("serial disconnect");
								}else{
									Parse("serial connect");
								}
								b.setActive(Init.getSConnection().isConnected());
							}
						});
					}
				});
				commandsWindow.add(b);
			}
		}else if(tab == STATUS_TAB){
			a.add(new A("Enable Sonar", "", "send ns 1"));
			a.add(new A("Disable Sonar", "", "send ns 0"));
		    a.add(new A("Enable Exploration", "", "send nE 1"));
		    a.add(new A("Disable Exploration", "", "send nE 0"));
		    a.add(new A("Stop moving", "", "send mS"));
		    a.add(new A("Allow Movement", "", "send mM 1"));
		    a.add(new A("Disable Movement", "", "send mM 0"));
		    a.add(new A("Allow Pathing", "", "send nP 1"));
		    a.add(new A("Disable Pathing", "", "send nP 0"));
		    
		}else if(tab == SETTINGS_TAB){
			a.add(new A("Refresh PIDs", "", "send Pl"));
		}
		
		Button b;
		for(final A c : a){
			b = new Button(c.name);
			b.setTooltipContent(c.desc);
			b.addCallback(new Runnable() {
				@Override
				public void run() {
					executor.submit(new Runnable() {
						@Override
						public void run() {
							Parse(c.command);
						}
					});
				}
			});
			commandsWindow.add(b);
		}
		
		if(commandsWindow.getNumChildren() > 0){
			commandsWindow.setVisible(true);
		}
	}
	
	private void selectTab(int tab){
		selectedTabInd = tab;
		loadCommands(selectedTabInd);
		tabModel.setValue(selectedTabInd);
		
		if(selectedTabInd == SETTINGS_TAB){
			PIDsWindow.setVisible(true);
			reloadPIDs();
		}else{
			PIDsWindow.setVisible(false);
		}
		
		if(selectedTabInd == STATUS_TAB){
			speedField.setVisible(true);
		}else{
			speedField.setVisible(false);
		}
	}
	
	@Override
    protected void createRootPane() {
        super.createRootPane();
        
        mapTabB = new ToggleButton("Map");
        statusTabB = new ToggleButton("Status");
        settingsTabB = new ToggleButton("Settings");
        
        tabModel = new SimpleIntegerModel(0, 2, 0);
        mapTabB.setModel(new OptionBooleanModel(tabModel, MAP_TAB));
        statusTabB.setModel(new OptionBooleanModel(tabModel, STATUS_TAB));
        settingsTabB.setModel(new OptionBooleanModel(tabModel, SETTINGS_TAB));
        
        mapTabB.addCallback(new Runnable() {
			@Override
			public void run() {
				pick.play();
				selectTab(MAP_TAB);
			}
        });
        
        statusTabB.addCallback(new Runnable() {
			@Override
			public void run() {
				pick.play();
				selectTab(STATUS_TAB);
			}
        });
        
        settingsTabB.addCallback(new Runnable() {
			@Override
			public void run() {
				pick.play();
				selectTab(SETTINGS_TAB);
			}
        });
        
        options = new Button("Options");
        options.addCallback( new Runnable(){
			@Override
			public void run() {
				pick.play();
				GUI.game.enterState(GUI.OPTIONSSTATE);
			}
		});
        
        controllers = new Button("Controllers");
        controllers.addCallback( new Runnable(){
			@Override
			public void run() {
				pick.play();
				Parser.parse("controllers");
			}
		});
        
        	console = new Widget();
			
			consoleLabel = new Label("Console");
			console.add(consoleLabel);
			
			consoleField = new EditField();
			consoleField.setForwardUnhandledKeysToCallback(true);
//			consoleField.setAutoCompletion(new Parser());//TODO fix: it does not show
			consoleField.addCallback(new Callback() {
				@Override
				public void callback(int key) {
					if(key == Input.KEY_ENTER || key == Input.KEY_NUMPADENTER){
						consoleBufferIn.append(consoleField.getText() + "\n");
						
						consoleFieldOldPos = -1;
						if(!consoleField.getText().equals("")){
							consoleFieldOld.remove(consoleField.getText());
							consoleFieldOld.add(0, consoleField.getText());
							if(consoleFieldOld.size() > 30){
								consoleFieldOld.subList(29, consoleFieldOld.size()).clear();
							}
						}
						
						consoleField.setText("");
					}else if(key == Input.KEY_UP){
						if(consoleFieldOldPos < consoleFieldOld.size()-1){
							consoleField.setText(consoleFieldOld.get(++consoleFieldOldPos));
						}
					}else if(key == Input.KEY_DOWN){
						if(consoleFieldOldPos > 0){
							consoleField.setText(consoleFieldOld.get(--consoleFieldOldPos));
						}else{
							consoleFieldOldPos = -1;
							consoleField.setText("");
						}
					}
				}
			});
			console.add(consoleField);
			
			consoleArea = new TextArea();
			consoleModel = new SimpleTextAreaModel();
			consoleArea.setModel(consoleModel);
			
			consoleScrollPane = new ScrollPane(consoleArea);
			consoleScrollPane.setFixed(ScrollPane.Fixed.HORIZONTAL);
			console.add(consoleScrollPane);
			
		console2 = new Widget();
		
		console2Label = new Label("Remote");
		console2.add(console2Label);
		
		console2Field = new EditField();
		console2Field.setForwardUnhandledKeysToCallback(true);
		//TODO console2Field.setAutoCompletion(dataSource); with remote command list
		console2Field.addCallback(new Callback() {
			@Override
			public void callback(int key) {
				if(key == Input.KEY_ENTER || key == Input.KEY_NUMPADENTER){
					if(Parser.parse("send " + console2Field.getText()) != 0){
						error.play();
					}
					
					console2FieldOldPos = -1;
					if(!console2Field.getText().equals("")){
						console2FieldOld.remove(console2Field.getText());
						console2FieldOld.add(0, console2Field.getText());
						if(console2FieldOld.size() > 30){
							console2FieldOld.subList(29, console2FieldOld.size()).clear();
						}
					}
					
					console2Field.setText("");
				}else if(key == Input.KEY_UP){
					if(console2FieldOldPos < console2FieldOld.size()-1){
						console2Field.setText(console2FieldOld.get(++console2FieldOldPos));
					}
				}else if(key == Input.KEY_DOWN){
					if(console2FieldOldPos > 0){
						console2Field.setText(console2FieldOld.get(--console2FieldOldPos));
					}else{
						console2FieldOldPos = -1;
						console2Field.setText("");
					}
				}
			}
		});
		console2.add(console2Field);
		
		console2Area = new TextArea();
		console2Model = new SimpleTextAreaModel();
		console2Area.setModel(console2Model);
		
		console2ScrollPane = new ScrollPane(console2Area);
		console2ScrollPane.setFixed(ScrollPane.Fixed.HORIZONTAL);
		console2.add(console2ScrollPane);
		
		consoleHeight = new DraggableButton();
		consoleHeight.setTheme("vscrollbar.thumb");
		consoleHeight.setListener(new DragListener() {
			private int y, h, y1, h1, y2, h2;
			
			@Override
			public void dragged(int deltaX, int deltaY) {
				if(consoleHeight.isVisible() && h - deltaY < 60){
					consoleHeight.setPosition(consoleHeight.getX(), y);
					consoleHeight.setSize(consoleHeight.getWidth(), h);
					
					console.setPosition(console.getX(), y1);
					console.setSize(console.getWidth(), h1);
					
					console2.setPosition(console2.getX(), y2);
					console2.setSize(console2.getWidth(), h2);
					
					hideConsole();
				}else if(!consoleHeight.isVisible() && h - deltaY > 60){
					showConsole();
				}
				if(consoleHeight.isVisible()){
					consoleHeight.setPosition(consoleHeight.getX(), y + deltaY);
					consoleHeight.setSize(consoleHeight.getWidth(), h - deltaY);
					
					console.setPosition(console.getX(), y1 + deltaY);
					console.setSize(console.getWidth(), h1 - deltaY );
					
					console2.setPosition(console2.getX(), y2 + deltaY);
					console2.setSize(console2.getWidth(), h2 - deltaY);
				}
			}
			
			@Override
			public void dragStopped() {
				rootPane.invalidateLayout();
			}
			
			@Override
			public void dragStarted() {
				y = consoleHeight.getY();
				h = consoleHeight.getHeight();
				y1 = console.getY();
				h1 = console.getHeight();
				y2 = console2.getY();
				h2 = console2.getHeight();
			}
		});
		
		commandsWindow = new Widget();
	    loadCommands(selectedTabInd);
	    
	    PIDsWindow = new Widget();
	    PIDsWindow.setVisible(false);
	    
	    speedField = new EditField();
	    speedField.setVisible(false);
	    speedField.addCallback(new Callback() {
			@Override
			public void callback(int key) {
				if(key == Input.KEY_ENTER || key == Input.KEY_NUMPADENTER){
					pick.play();
					try{
						float f = Float.parseFloat(speedField.getText());
						
						int[] d = Device.writeFloat(f, new int[]{'m', 's'});
						kartobot.getC().send(d);
					}catch(NumberFormatException e){
						disp("Please use floats: " + e.getMessage());
					}
				}
			}
		});
		
		rootPane.add(console);
		rootPane.add(console2);
        rootPane.add(options);
        rootPane.add(controllers);
        rootPane.add(mapTabB);
        rootPane.add(statusTabB);
        rootPane.add(settingsTabB);
        rootPane.add(consoleHeight);
        rootPane.add(commandsWindow);
        rootPane.add(PIDsWindow);
        rootPane.add(speedField);
    }
	
	private void layoutPIDWindow(){
		int maxWidth = 0, width = 0, totalHeight = 0, maxHeight = 0;
        for(int i=0; i<PIDsWindow.getNumChildren(); i++){
			Widget b = PIDsWindow.getChild(i);

    		b.adjustSize();
    		width += b.getWidth() + 10;
    		if(i % 6 == 4 ||  i % 6 == 5){
    			width -= 15;
    		}
    		maxHeight = Math.max(maxHeight, b.getHeight());
    		maxWidth = Math.max(maxWidth, width);
    		
    		if(i % 6 == 5){
    			if(totalHeight != 0){
        			totalHeight += b.getHeight()*1.7;
        		}else{
        			totalHeight += b.getHeight();
        		}
    			width = 0;
    		}
    	}
    	
        PIDsWindow.setSize(maxWidth + 40, totalHeight + maxHeight*2);
        PIDsWindow.setPosition(100, 100);
        
		int y = PIDsWindow.getY() + 5;
		maxHeight = 0; width = 20;
		for(int i=0; i<PIDsWindow.getNumChildren(); i++){
			Widget b = PIDsWindow.getChild(i);
			maxHeight = Math.max(maxHeight, b.getHeight());
			
			if(i % 6 == 0){
	    		if(i > 0){
	    			y += maxHeight*1.7;
	    		}else{
	    			y += maxHeight;
	    		}
	    		maxHeight = 0;
	    		width = 20;
			}
			
			if(i > 5){
    			width = PIDsWindow.getChild(i % 6).getX() - PIDsWindow.getX();
    			b.setSize(PIDsWindow.getChild(i % 6).getWidth(), b.getHeight());
    		}
			
			if(i % 6 == 4 || i % 6 == 5){
    			b.setPosition(PIDsWindow.getX() + width, y - 1);
    		}else{
    			b.setPosition(PIDsWindow.getX() + width, y);
    		}
    		
    		if(i <= 5){
    			width += 10 + b.getWidth();
    		}
    		if(i % 6 == 4){
    			width -= 18;
    		}
    	}
	}
	
	private boolean firstLayout = true;
    @Override
    protected void layoutRootPane(){
    	if(firstLayout){
    		mapTabB.adjustSize();
	    	statusTabB.adjustSize();
	    	settingsTabB.adjustSize();
	    	
	    	options.adjustSize();
	    	controllers.adjustSize();
	    	
	    	console.setSize(GUI.container.getWidth()/2, GUI.container.getHeight()/3);
	    	consoleField.adjustSize();
	    	consoleField.setSize(console.getWidth() - 18, consoleField.getHeight());
	    	consoleLabel.adjustSize();
	    	
	    	console2.setSize(GUI.container.getWidth()/2, console.getHeight());
	    	console2Field.adjustSize();
	    	console2Field.setSize(console2.getWidth() - 18, console2Field.getHeight());
	    	console2Label.adjustSize();
	    	
	    	consoleHeight.adjustSize();
	    	
	    	speedField.adjustSize();
	    	speedField.setSize(80, speedField.getHeight());
	    	speedField.setPosition(GUI.container.getWidth()/2 - speedField.getWidth()/2, 70);
    		
			firstLayout = false;
    	}
	    	
		int xLoc = (GUI.container.getWidth() - mapTabB.getWidth() - statusTabB.getWidth() - settingsTabB.getWidth() + 10) / 2;
		mapTabB.setPosition(xLoc, 0);
		statusTabB.setPosition(xLoc + mapTabB.getWidth() - 10, 0);
		settingsTabB.setPosition(xLoc + mapTabB.getWidth() + statusTabB.getWidth() - 20, 0);
		
		options.setPosition(GUI.container.getWidth() - options.getWidth() , 0);
		controllers.setPosition(options.getX() - controllers.getWidth() + 10, 0);
		
		console.setPosition(0, GUI.container.getHeight() - console.getHeight());
		consoleScrollPane.setPosition(console.getX() + 10, console.getY() + 9);
		consoleField.setPosition(console.getX() + 9, console.getY() + console.getHeight() - consoleField.getHeight() - 3);
		consoleLabel.setPosition(console.getX() + console.getWidth()/2 - consoleLabel.getWidth()/2, console.getY() - 10);
		
		console2.setPosition(GUI.container.getWidth()/2, GUI.container.getHeight() - console2.getHeight());
		console2ScrollPane.setPosition(console2.getX() + 10, console2.getY() + 9);
		console2Field.setPosition(console2.getX() + 9, console2.getY() + console2.getHeight() - console2Field.getHeight() - 3);
		console2Label.setPosition(console2.getX() + console2.getWidth()/2 - console2Label.getWidth()/2, console2.getY() - 10);
			
		consoleHeight.setPosition(console.getX() + console.getWidth() - consoleHeight.getWidth()/2, console.getY() + 8);
		
		consoleScrollPane.setSize(console.getWidth() - 20, console.getHeight() - consoleField.getHeight() - 13);
		console2ScrollPane.setSize(console2.getWidth() - 20, console2.getHeight() - console2Field.getHeight() - 13);
		consoleHeight.setSize(consoleHeight.getWidth(), console.getHeight() - 16);
		
		{
			int maxWidth = 0, totalHeight = 0;
	        Widget last = null;
	        for(int i=0; i<commandsWindow.getNumChildren(); i++){
				Widget b = commandsWindow.getChild(i);

	    		b.adjustSize();
	    		if(totalHeight != 0){
	    			totalHeight+=b.getHeight()*1.8;
	    		}else{
	    			totalHeight+=b.getHeight();
	    		}
	    		
	    		maxWidth = Math.max(maxWidth, b.getWidth());
	    		last = b;
	    	}
	    	
	        commandsWindow.setSize((int)(maxWidth*1.5), totalHeight + last.getHeight()*2);
	        commandsWindow.setPosition(GUI.container.getWidth() - commandsWindow.getWidth(), 100);
	        
			int y = commandsWindow.getY();
			for(int i=0; i<commandsWindow.getNumChildren(); i++){
				Widget b = commandsWindow.getChild(i);
	    		if(y != commandsWindow.getY()){
	    			y+=b.getHeight()*1.8;
	    		}else{
	    			y+=b.getHeight();
	    		}
	    		b.setPosition(commandsWindow.getX() +commandsWindow.getWidth()/2 - b.getWidth()/2, y);
	    	}
		}
		layoutPIDWindow();
    }
    
    @Override
    public void reloadRootPane(){
    	firstLayout = true;
    	super.reloadRootPane();
    }
    
    public void mouseMoved(int oldX, int oldY, int newX, int newY) {
	}
    
    @Override
    public void mouseDragged(int oldX, int oldY, int newX, int newY) {
    }
    
    @Override
    public void mouseClicked(int button, int x, int y, int clickCount) {
    	if(button == Input.MOUSE_LEFT_BUTTON && clickCount == 1){
    		
    	}
	}
    
    @Override
    public void mousePressed(int button, int x, int y) {
	}
    
    @Override
    public void mouseReleased(int button, final int x, final int y) {
    	if(selectedTabInd == MAP_TAB){
	    	if(button == Input.MOUSE_LEFT_BUTTON ){
	    		
	    	}else if((button == Input.MOUSE_RIGHT_BUTTON)){
	    		int tx = (int) (((GRID_SQUARE_SIZE/2 + x+screenX - (x+screenX) % GRID_SQUARE_SIZE) / GRID_SQUARE_SIZE) / zoom);
	    		int ty = (int) (((GRID_SQUARE_SIZE/2 + y+screenY - (y+screenY) % GRID_SQUARE_SIZE) / GRID_SQUARE_SIZE) / zoom);
	    		consoleAppend(rmbCommand.useDesc + " " + tx + " " + ty + "\n");
	    		if(tx >= 0 && tx < GRID_MAX_SIZE && ty >= 0 && ty < GRID_MAX_SIZE){
	    			Parse(rmbCommand.command + " " + tx + " " + ty);
	    		}
	    	}
    	}
	}
    
    public void mouseWheelMoved(int newValue) {
    	float zoomMult = 1 + ZOOM_CHANGE_MULT * Math.signum(newValue);
    	screenX += (input.getMouseX() - input.getMouseX() / zoomMult) / zoom;
    	screenY += (input.getMouseY() - input.getMouseY() / zoomMult) / zoom;
    	zoom *= zoomMult;
    	dirtyGrid = true;
//    	System.out.println("Screen = (" + screenX + ", " + screenY + ")\n" + 
//    					   "Mouse = (" + input.getMouseX() + ", " + input.getMouseY() + ")\n" + 
//    					   "--");
	}
    
    private void showConsole(){
    	console.setVisible(true);
		console2.setVisible(true);
		consoleHeight.setVisible(true);
		console.requestKeyboardFocus();
    }
    
    private void hideConsole(){
    	console.setVisible(false);
		console2.setVisible(false);
		consoleHeight.setVisible(false);
    }
    
    @Override
    public void keyPressed(int key, char c) {    	
    	if(key == Input.KEY_ESCAPE){
			GUI.game.enterState(GUI.OPTIONSSTATE);
    	}else if(key == Input.KEY_C){
    		if(console.isVisible()){
    			hideConsole();
    		}else{
    			showConsole();
    		}
    	}else if(key == Input.KEY_Z){
    		zoom = 2;
    		screenX = 0;
    		screenY = 0;
    		dirtyGrid = true;
    	}
	}
	
}