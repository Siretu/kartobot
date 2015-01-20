package se.exuvo.kartobotpc.gui;


import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.List;

import javax.swing.DefaultListModel;
import javax.swing.Icon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.SpringLayout;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import se.exuvo.kartobotpc.Init;
import se.exuvo.kartobotpc.Input.Bind;
import se.exuvo.kartobotpc.Input.InputListener;
import se.exuvo.kartobotpc.commands.Controllers.Tab;

import net.java.games.input.Component;
import net.java.games.input.Controller;
import net.java.games.input.Event;

public class Binds extends JPanel implements Tab, ListSelectionListener, ActionListener, InputListener{
	private static final long serialVersionUID = -3900502184260885964L;
	private JPanel jContentPane;
	private JPanel bindPane;
	private JList list;
	private JScrollPane listScroll;
	private DefaultListModel listContent;
	private JButton save;
	private JButton reset;
	private JButton remove;
	private JTextField prefix;
	private JTextField value;
	private JTextField name;
	private JTextField inMax;
	private JTextField inMin;
	private JTextField deadzone;
	private JTextField outMax;
	private JTextField outMin;
	private JTextField trim;
	private JTextField code;
	private JCheckBox isBoolean;
	private JCheckBox isToggle;
	private Bind selectedBind = null;
	private Component selectedComponent = null;
	private Controller selectedController = null;
	
	public Binds(){
		super();
		this.setName("Binds");
	}

	@Override
	public JPanel getTab() {
		Init.getInput().addEventListener(this);
		return getJContentPane();
	}
	
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jContentPane = new JPanel();
			jContentPane.setLayout(new BorderLayout());
			
			listContent = new DefaultListModel();
			for(Controller c : Init.getInput().getControllers()){
				listContent.addElement(c);
			}
			
			list = new JList();
			list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
			list.setLayoutOrientation(JList.VERTICAL);
			//jList.setVisibleRowCount(-1);
			list.setModel(listContent);
			list.addListSelectionListener(this);
			listScroll = new JScrollPane(list);
			listScroll.setPreferredSize(new Dimension(200, 80));
			jContentPane.add(listScroll, BorderLayout.WEST);
			
			jContentPane.add(getBindPane(), BorderLayout.EAST);
		}
		return jContentPane;
	}
	
	private JPanel getBindPane() {
		if (bindPane == null) {
			List<java.awt.Component> stuff = new ArrayList<java.awt.Component>();
			
			bindPane = new JPanel();
			SpringLayout layout = new SpringLayout();
			bindPane.setLayout(layout);
			
				save = new JButton("save");
				save.addActionListener(this);
				
				remove = new JButton("remove");
				remove.addActionListener(this);
				
				reset = new JButton("reset");
				reset.addActionListener(this);
				
				bindPane.add(save);
				bindPane.add(remove);
				bindPane.add(reset);
			
			prefix = new JTextField();
			prefix.setName("Prefix: ");
			prefix.setToolTipText("String to send before value");
			value = new JTextField();
			value.setEditable(false);
			value.setName("Value: ");
			name = new JTextField();
			name.setName("Name: ");
			inMin = new JTextField();
			inMin.setName("Min In: ");
			inMax = new JTextField();
			inMax.setName("Max In: ");
			deadzone = new JTextField();
			deadzone.setName("Deadzone: ");
			outMin = new JTextField();
			outMin.setName("Min Out: ");
			outMax = new JTextField();
			outMax.setName("Max Out: ");
			trim = new JTextField();
			trim.setName("Trim: ");
			code = new JTextField();
			code.setName("Code: ");
			isBoolean = new JCheckBox();
			isBoolean.setName("Boolean Output: ");
			isBoolean.setToolTipText("If true, will send only prefix if input>0");
			isToggle = new JCheckBox();
			isToggle.setName("Toggle: ");
			isToggle.setEnabled(false);
			
			stuff.add(name);
			stuff.add(prefix);
			stuff.add(value);
			stuff.add(inMin);
			stuff.add(inMax);
			stuff.add(deadzone);
			stuff.add(outMin);
			stuff.add(outMax);
			stuff.add(trim);
			stuff.add(code);
			stuff.add(isBoolean);
			stuff.add(isToggle);
			
			layout.putConstraint(SpringLayout.SOUTH, save,0,SpringLayout.SOUTH, bindPane);
			layout.putConstraint(SpringLayout.SOUTH, remove,0,SpringLayout.SOUTH, bindPane);
			layout.putConstraint(SpringLayout.SOUTH, reset,0,SpringLayout.SOUTH, bindPane);
				layout.putConstraint(SpringLayout.WEST, save,0,SpringLayout.WEST, bindPane);
				layout.putConstraint(SpringLayout.WEST, remove,0,SpringLayout.EAST, save);
				layout.putConstraint(SpringLayout.WEST, reset,0,SpringLayout.EAST, remove);
				layout.putConstraint(SpringLayout.EAST, reset,0,SpringLayout.EAST, bindPane);
			
			java.awt.Component last = bindPane;
			for(java.awt.Component c : stuff){
				bindPane.add(c);
				JLabel label = new JLabel(c.getName());
				bindPane.add(label);
				if(last == bindPane){
					layout.putConstraint(SpringLayout.NORTH, c,0,SpringLayout.NORTH, last);
				}else{
					layout.putConstraint(SpringLayout.NORTH, c,0,SpringLayout.SOUTH, last);
				}
				layout.putConstraint(SpringLayout.NORTH, label,0,SpringLayout.NORTH, c);
				layout.putConstraint(SpringLayout.WEST, label,0,SpringLayout.WEST, bindPane);
				layout.putConstraint(SpringLayout.WEST, c,0,SpringLayout.EAST, label);
				layout.putConstraint(SpringLayout.EAST, c,0,SpringLayout.EAST, bindPane);
				
				last = c;
			}
			
			bindPane.setPreferredSize(new Dimension(250, 100));
			clear();
		}
		return bindPane;
	}

	@Override
	public String getDescription() {
		return "Binds inputs to commands";
	}

	@Override
	public Icon getIcon() {
		// TODO Auto-generated method stub
		return null;
	}
	
	private void clear(){
		prefix.setText("");
		value.setText("NULL");
		name.setText("");
		inMin.setText("0");
		inMax.setText("0");
		deadzone.setText("0");
		outMin.setText("0");
		outMax.setText("0");
		trim.setText("0");
		code.setText("");
		isBoolean.setSelected(false);
		isToggle.setSelected(false);
	}
	
	private boolean read(){
		if(selectedBind != null){
			Bind b = selectedBind;
			prefix.setText(b.prefix);
			name.setText(b.name);
			inMax.setText("" + b.in_max);
			inMin.setText("" + b.in_min);
			deadzone.setText("" + b.deadzone);
			trim.setText("" + b.trim);
			code.setText("" + b.code);
			outMax.setText("" + b.out_max);
			outMin.setText("" + b.out_min);
			isBoolean.setSelected(b.isBoolean);
			isToggle.setSelected(b.isToggle);
			value.setText(selectedComponent.getPollData() + " (" + b.scale(selectedComponent.getPollData()) + ")");
			if(isBoolean.isSelected()){
				value.setText(value.getText() + "=" + (b.scale(selectedComponent.getPollData()) > 0?true:false));
			}
			return true;
		}else{
			clear();
			value.setText("" + selectedComponent.getPollData());
		}
		return false;
	}

	@Override
	public void valueChanged(ListSelectionEvent e) {
		if(e.getSource() == list && !e.getValueIsAdjusting() && list.getSelectedIndex() != -1){
			if(selectedComponent != null){//Binds list
				if(list.getSelectedIndex() != listContent.size()-1){
					selectedBind = (Bind) listContent.get(list.getSelectedIndex());
					//TODO select
					read();
				}else{
					clear();
					listContent.clear();
					selectedBind = null;
					selectedComponent = null;
					for(Component cc : selectedController.getComponents()){
						listContent.addElement(cc);
					}
					listContent.addElement("[Back to Controllers]");
				}
			}else if(selectedController != null){//Component list
				if(list.getSelectedIndex() != listContent.size()-1){
					selectedComponent = (Component) listContent.get(list.getSelectedIndex());
					listContent.clear();
					for(Bind b : Init.getInput().getBinds()){
						if(b.c == selectedComponent){
							listContent.addElement(b);
						}
					}
					listContent.addElement("[Back to Components]");
					read();
				}else{
					listContent.clear();
					selectedController = null;
					for(Controller c : Init.getInput().getControllers()){
						listContent.addElement(c);
					}
					clear();
				}
			}else{//Controller list
				selectedController = (Controller) listContent.get(list.getSelectedIndex());
				listContent.clear();
				for(Component cc : selectedController.getComponents()){
					listContent.addElement(cc);
				}
				listContent.addElement("[Back to Controllers]");
			}
		}
	}
	
	private void listRefresh(){
		listContent.clear();
		for(Bind b : Init.getInput().getBinds()){
			if(b.c == selectedComponent){
				listContent.addElement(b);
			}
		}
		listContent.addElement("[Back to Components]");
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if(e.getSource() == save){
			Init.getInput().getBinds().remove(selectedBind);
			
			Bind b = Init.getInput().new Bind(selectedController, selectedComponent);
			b.prefix = prefix.getText();
			b.name = name.getText();
			b.in_max = inMax.getText().equals("")? 0:Float.parseFloat(inMax.getText());
			b.in_min = inMin.getText().equals("")? 0:Float.parseFloat(inMin.getText());
			b.deadzone = deadzone.getText().equals("")? 0:Float.parseFloat(deadzone.getText());
			b.out_max = outMax.getText().equals("")? 0:Integer.parseInt(outMax.getText());
			b.out_min = outMin.getText().equals("")? 0:Integer.parseInt(outMin.getText());
			b.isBoolean = isBoolean.isSelected();
			b.isToggle = isToggle.isSelected();
			b.trim = trim.getText().equals("")? 0:Integer.parseInt(trim.getText());
			b.code = code.getText();
			Init.getInput().addBind(b);
			listRefresh();
			selectedBind = b;
		}else if(e.getSource() == reset){
			read();
		}else if(e.getSource() == remove && selectedBind != null){
			Init.getInput().getBinds().remove(selectedBind);
			selectedBind = null;
			read();
			listRefresh();
		}
	}

	@Override
	public void inputChanged(Event e) {
		if(e.getComponent() == selectedComponent){
			value.setText("" + selectedComponent.getPollData());
			if(selectedBind != null){
				value.setText(value.getText() + " (" + selectedBind.scale(selectedComponent.getPollData()) + ")");
				if(isBoolean.isSelected()){
					value.setText(value.getText() + "=" + (selectedBind.scale(selectedComponent.getPollData()) > 0?true:false));
				}
			}
		}
	}

}
