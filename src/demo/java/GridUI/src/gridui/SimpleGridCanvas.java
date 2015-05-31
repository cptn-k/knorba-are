/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package gridui;

import java.awt.*;

/**
 *
 * @author hamed
 */
public class SimpleGridCanvas extends net.hkhandan.gui.AbstractGridCanvas {

    private static class Cell {
        public Color color;
    }
    
    Cell[][] cells;
    
    public void setCellColor(int i, int j, Color c) {
        if(cells[i][j] == null) {
            cells[i][j] = new Cell();
        }
        
        cells[i][j].color = c;
    }
    
    @Override 
    public void setGridSize(int width, int height) {
        super.setGridSize(width, height);
        if(width == 0 || height == 0) {
            return;
        }
        cells = new Cell[width][height];
    }
    
    
    @Override
    protected void drawCell(int col, int row, Graphics g) {
        if(cells == null) {
            return;
        }
        
        if(cells[col][row] == null) {
            return;
        }
        
        g.setColor(cells[col][row].color);
        Rectangle r = g.getClipBounds();
        g.fillRect(r.x, r.y, r.width, r.height);
    }
    
}
