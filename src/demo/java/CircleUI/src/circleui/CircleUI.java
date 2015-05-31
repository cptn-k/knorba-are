/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package circleui;

import java.awt.*;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.IOException;
import java.net.URL;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;

/**
 *
 * @author hamed
 */
public class CircleUI extends Frame {
    
//// NESTED TYPES /////////////////////////////////////////////////////////////

    private static class EventHandler implements WindowListener {
        @Override
        public void windowOpened(WindowEvent e) {
            // Nothing;
        }

        @Override
        public void windowClosing(WindowEvent e) {
            System.exit(0);
        }

        @Override
        public void windowClosed(WindowEvent e) {
            // Nothing;
        }

        @Override
        public void windowIconified(WindowEvent e) {
            // Nothing;
        }

        @Override
        public void windowDeiconified(WindowEvent e) {
            // Nothing;
        }

        @Override
        public void windowActivated(WindowEvent e) {
            // Nothing;
        }

        @Override
        public void windowDeactivated(WindowEvent e) {
            // Nothing;
        }
    }
    
    private static class Circle {
        private int x;
        private int y;
        private int r;
        
        public Circle() {
            this(0, 0, 0);
        }
        
        public Circle(int x, int y, int r) {
            this.x = x;
            this.y = y;
            this.r = r;
        }
        
        public int getX() {
            return x;
        }
        
        public int getY() {
            return y;
        }
        
        public int getR() {
            return r;
        }
        
        public void set(int x, int y, int r) {
            this.x = x;
            this.y = y;
            this.r = r;
        }
        
        public String toString() {
            return String.format("(%d, %d, %d)", x, y, r);
        }
    } // class Circle
    
    
//// FIELDS ///////////////////////////////////////////////////////////////////
    
    Circle[] display = new Circle[5000];
    Circle[] buffer = new Circle[5000];
       
    int nDisplay = 0;
    int nBuffer = 0;
    Image bubbleImage = null;

    
//// METHODS //////////////////////////////////////////////////////////////////
    
    public CircleUI() throws IOException {
        ClassLoader ldr = CircleUI.class.getClassLoader();
        URL url = ldr.getResource("bubble.png");
        if(url != null) {
            bubbleImage =  ImageIO.read(url);
        }
    }
    
    void addadasdasd(int x, int y, int r) {
        if(buffer[nBuffer] == null) {
            buffer[nBuffer] = new Circle(x, y, r);
        } else {
            buffer[nBuffer].set(x, y, r);
        }
        
        nBuffer++;
    }
    
    void commit() {
        Circle[] tmp = display;
        display = buffer;
        nDisplay = nBuffer;
        
        buffer = tmp;
        nBuffer = 0;       
        
        repaint();
    }
        
    @Override
    public void paint(Graphics g) {
        g.clearRect(0, 0, getWidth(), getHeight());
        
        g.setColor(Color.BLACK);
        
        for(int i = 0; i < nDisplay; i++) {
            Circle c = display[i];
            if(bubbleImage != null) {
                g.drawImage(bubbleImage, c.x - c.r, c.y - c.r, c.x + c.r, c.y + c.r, 0, 0, 220, 220, null);
            } else {
                g.drawOval(c.x - c.r, c.y - c.r, c.r * 2, c.r * 2);
            }
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        CircleUI ui = null;
        try {
            ui = new CircleUI();
        } catch (IOException ex) {
            Logger.getLogger(CircleUI.class.getName()).log(Level.SEVERE, null, ex);
            System.exit(1);
        }
        
        if(args.length == 4) {
            int x = Integer.parseInt(args[0]);
            int y = Integer.parseInt(args[1]);
            int w = Integer.parseInt(args[2]);
            int h = Integer.parseInt(args[3]);
            ui.setLocation(x, y);
            ui.setSize(w, h + 10);
        } else {
            ui.setSize(400, 300);
        }
        
        ui.addWindowListener(new EventHandler());
        
        ui.setVisible(true);
        
        StringBuilder builder = new StringBuilder();
        
        int x = 0;
        int y = 0;
        int r = 0;
        
        while(true) {
            int ch = -1;
            
            try {
              ch = System.in.read();
            } catch(IOException e) {
                e.printStackTrace(System.err);
            }
            
            if(ch == -1) {
                System.out.println("End of standard input pipe. Bye.");
                System.exit(0);
            }
            
            switch(ch) {
                case 'x':
                    x = Integer.parseInt(builder.toString());
                    builder.setLength(0);
                    break;
                    
                case 'y':
                    y = Integer.parseInt(builder.toString()) + 10;
                    builder.setLength(0);
                    break;
                    
                case 'r':
                    r = Integer.parseInt(builder.toString());
                    builder.setLength(0);
                    break;
                    
                case 'a':
                    ui.add(x, y, r);
                    x = y = r = 0;
                    builder.setLength(0);
                    break;
                    
                case '\n':
                    ui.commit();
                    builder.setLength(0);
                    break;
                    
                case ' ':
                    break;
                    
                case 'c':
                    builder.setLength(0);
                    break;
                    
                default:
                    builder.append((char)ch);
            }
        }
        
    }
    
}
