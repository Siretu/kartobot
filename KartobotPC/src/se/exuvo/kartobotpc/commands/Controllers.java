package se.exuvo.kartobotpc.commands;

import javax.swing.Icon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import se.exuvo.kartobotpc.gui.Binds;


public class Controllers extends Command {
	
	public Controllers(){
		super("controllers", "Configure input devices");
	}

	@Override
	public int execute(String[] tokens, String phrase) {
		JFrame window = new JFrame();
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		} catch (InstantiationException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (UnsupportedLookAndFeelException e) {
			e.printStackTrace();
		}
		window.setSize(640, 480);
		JTabbedPane tabbedPane = new JTabbedPane();
		addTab(new Binds(), tabbedPane);
		window.setContentPane(tabbedPane);
		window.setTitle("Quad Controller");
		window.setLocationRelativeTo(null);
		window.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		window.setVisible(true);
		
		return 0;
	}
	
	public void addTab(Tab t, JTabbedPane p){
		p.addTab(t.getName(), t.getIcon(), t.getTab(), t.getDescription());
	}
	
	public interface Tab {

		public JPanel getTab();
		public String getName();
		public String getDescription();
		public Icon getIcon();
	}
	

}
