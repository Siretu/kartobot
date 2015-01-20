package se.exuvo.kartobotpc.gui;

import java.awt.Point;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.newdawn.slick.geom.Vector2f;

/**
 * Positive coordinate system with origo in the bottom left corner.
 * 
 * @author per
 * @created 12/1 - 2012
 * @updated 12/1 - 2012
 */
public class World {

	private int gridSquareSize,
				gridMaxSize;
	private int[][] worldMap;
	private List<Point> livingSquares;
	
	
	
	private void removeLivingSquare(Point square) {
		for(Iterator<Point> it = livingSquares.iterator(); it.hasNext();) {
			Point v = it.next();
			if(v.x == square.x && v.y == square.y) {
				it.remove();
				break;
			}
		}
	}
	
	
	
	public World(int gridSquareSize, int gridMaxSize) {
		this.gridSquareSize = gridSquareSize;
		this.gridMaxSize = gridMaxSize;
		worldMap = new int[gridMaxSize][gridMaxSize];
		livingSquares = new ArrayList<Point>();
	}
	
	
	
	public void addToLifeLength(Point square, int additionalLife) {
		if(worldMap[square.x][square.y] == 0) {
			livingSquares.add(square);
		}
		worldMap[square.x][square.y] += additionalLife;
	}
	
	public void subFromLifeLength(Point square, int removedLife) {
		worldMap[square.x][square.y] -= removedLife;
		if(worldMap[square.x][square.y] == 0) {
			removeLivingSquare(square);
		}
	}
	
	
	
	public void setLifeLength(Point square, int newLife) {
		int diff = newLife - worldMap[square.x][square.y];
		if(diff > 0) {
			if(worldMap[square.x][square.y] == 0) {
				livingSquares.add(square);
			}
		} else if(diff < 0) {
			if(newLife == 0) {
				removeLivingSquare(square);
			}
		}
		worldMap[square.x][square.y] = newLife; 
	}
	
	public int getLifeLength(Point square) {
		return worldMap[square.x][square.y];
	}
	
	public int getSquareSize() {
		return gridSquareSize;
	}
	
	public int getGridMaxSize() {
		return gridMaxSize;
	}
}
