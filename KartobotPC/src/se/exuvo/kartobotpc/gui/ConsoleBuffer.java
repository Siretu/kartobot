package se.exuvo.kartobotpc.gui;


public class ConsoleBuffer {
	private StringBuffer buffer = new StringBuffer(32);
	private boolean hasLine = false;

	public void append(String s){
		synchronized(buffer){
			buffer.append(s);
			if(s.contains("\n")){
				hasLine = true;
			}
		}
	}
	
	public boolean hasLine(){
		return hasLine;
	}
	
	public String readLine(){
		int i;
		while(true){
			synchronized(buffer){
				i = buffer.indexOf("\n");
			}
			if(i == -1){
				Thread.yield();
			}else{
				break;
			}
		}
		String s;
		synchronized(buffer){
			s = buffer.substring(0, i+1);
			buffer.delete(0, i+1);
			hasLine = buffer.indexOf("\n") != -1;
		}
		return s;
	}
	
	public char read(){
		boolean sleep = true;
		while(sleep){
			synchronized(buffer){
				sleep = buffer.length() < 1;
			}
			if(sleep){
				Thread.yield();
			}
		}
		char c;
		synchronized(buffer){
			c = buffer.charAt(0);
			buffer.deleteCharAt(0);
			hasLine = buffer.indexOf("\n") != -1;
		}
		return c;
	}

}
