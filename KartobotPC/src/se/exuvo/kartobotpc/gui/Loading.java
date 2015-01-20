package se.exuvo.kartobotpc.gui;

import org.apache.log4j.Logger;
import org.newdawn.slick.Color;
import org.newdawn.slick.GameContainer;
import org.newdawn.slick.Graphics;
import org.newdawn.slick.Image;
import org.newdawn.slick.Music;
import org.newdawn.slick.SlickException;
import org.newdawn.slick.state.StateBasedGame;
import org.newdawn.slick.state.transition.FadeInTransition;
import org.newdawn.slick.state.transition.FadeOutTransition;

import se.exuvo.kartobotpc.ResourceManager;
import TWLSlick.BasicTWLGameState;
import de.matthiasmann.twl.ProgressBar;

/**
 * The second state of the game, a simple state to load resources. Like
 * presentation state this state load his own resources.
 * 
 * After loading all resources, the state move on the first view, the main menu
 * view.
 * 
 * @author Vincent PIRAULT
 * 
 */
public class Loading extends BasicTWLGameState {
	private static final Logger log = Logger.getLogger(Loading.class);

	private Image background;
	private ProgressBar bar;
	private boolean ready = false;
	
	@Override
	public void enter(GameContainer container, StateBasedGame game) throws SlickException {
		super.enter(container,game);
		log.debug("Entered Loading state");
		
		background = new Image("data/images/resources_view_background.jpg");
		//background = new Image("data/images/chaos_sphere_blue_800x600.png");
		ResourceManager.init("resource.jar");
		
		if(!container.isSoundOn() || !container.isMusicOn()){
			disp("Sound system failed to initialize. Restart might fix it.");
		}
	}
	
	@Override
	public void initResources(){
		
	}

	@Override
	public void init(GameContainer container, StateBasedGame game) throws SlickException {
		GUI.setBackgroundMusic(new Music("data/music/Megaman X/xm/Capsule.xm"));
		GUI.getBackgroundMusic().loop(0.9f, 0.7f);
		GUI.getBackgroundMusic().stop();
	}
	
	@Override
    protected void createRootPane() {
		super.createRootPane();
		
		bar = new ProgressBar();
		bar.setTheme("progressbar-glow-anim");
		bar.setSize(300, 10);
		bar.setPosition(GUI.container.getWidth() / 2 - 160, GUI.container.getHeight() / 2 - 20);
		rootPane.add(bar);
		
		//GUI.container.setMouseGrabbed(true);
	}

	@Override
	public void render(GameContainer container, StateBasedGame sbGame, Graphics g) throws SlickException {
		background.draw(0, 0, container.getWidth(), container.getHeight());
		g.setColor(Color.red);
		g.drawString("Loading ... " + ResourceManager.getProgress() + "%", container.getWidth() / 2 - 80, container.getHeight() / 2 - 60);

//		if (ready) {
//			g.drawString("Press a key or click", container.getWidth() / 2 - 90, container.getHeight() / 2 + 10);
//		}
	}

	@Override
	public void update(GameContainer container, StateBasedGame game, int delta) throws SlickException {
		long start = System.currentTimeMillis();
		while(System.currentTimeMillis() - start < 100 && !ResourceManager.isLoadComplete()){
			ResourceManager.loadNextResource();
		}
		
		if (ResourceManager.isLoadComplete() && !ready) {
			for (int i = 1; i <= game.getStateCount(); i++) {
				BasicTWLGameState state = (BasicTWLGameState) game.getState(i);
				state.initResources();
			}
	
			ready = true;
			goToMenu();
		}
		if (bar != null){
			bar.setValue(((float) ResourceManager.getProgress()) / 100);
		}
	}

	@Override
	public void keyPressed(int key, char c) {
		super.keyPressed(key, c);
		goToMenu();
	}

	@Override
	public void mousePressed(int button, int x, int y) {
		super.mousePressed(button, x, y);
		goToMenu();
	}

	private void goToMenu() {
		if (ready) {
			GUI.container.setMouseGrabbed(false);
			GUI.game.enterState(GUI.MAINSTATE);
		}
	}

	@Override
	public int getID() {
		return GUI.LOADINGSTATE;
	}


}

