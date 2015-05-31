/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package textui;

import java.awt.*;
import java.awt.event.*;
import java.io.IOException;
import javax.swing.*;

/**
 *
 * @author hamed
 */
public class TextUI {

// --- NESTED TYPES --- //
    
    static class EventHandler implements WindowListener {
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
    
    
    private static class UiFrame extends JFrame {
        private JLabel label;
        private Color c;

        
        public UiFrame() {
            setupComponents();
        }
        
        
        public UiFrame(GraphicsConfiguration conf) {
            super(conf);
            setupComponents();
        }
        
        
        private void setupComponents() {
            c = getBackground();
            addWindowListener(new EventHandler());        
        
            setLayout(new BorderLayout());
            label = new JLabel();
            label.setText("");
            label.setHorizontalAlignment(JLabel.CENTER);
            label.setVerticalAlignment(JLabel.CENTER);
            label.setFont(new Font("SansSerif", Font.PLAIN, 24));
            label.setOpaque(true);
            
            add(label, BorderLayout.CENTER);        
        }

        
        @Override
        public void invalidate() {
            int h = getHeight();
            if(h == 0) {
                h = 200;
            }
            
            int fontSize = (int)(((double)h/200) * 24);      
            label.setFont(new Font("SansSerif", Font.PLAIN, fontSize));
            super.invalidate();
        }

        
        public void display(String s) {
            label.setText(s);
        }
        
        
        public void setColor(Color c) {
            label.setBackground(c);
        }
    }
    
    
    class ColorAnimator extends Thread {
        @Override
        public void run() {
            int myPriority = ++_animatorPriority;
            for(int i = 0; i < 255; i += 10) {
                for(int f = _frames.length - 1; f >= 0; f--) {
                    _frames[f].setColor(new Color(i, i, 255));
                }
                                
                if(myPriority < _animatorPriority) {
                    break;
                }
                
                try {
                    sleep(20);
                } catch (InterruptedException ex) {
                    // Nothing
                }
            }
        }
    }
    
    
// --- FIELDS --- //
        
    int _animatorPriority = -1;
    private UiFrame[] _frames;
    
    
// --- CONSTRUCTORS --- //
    
    public TextUI() {
        GraphicsEnvironment gr = GraphicsEnvironment.getLocalGraphicsEnvironment();
        GraphicsDevice[] screens = gr.getScreenDevices();
        
        _frames = new UiFrame[screens.length];
        
        for(int i = 0; i < _frames.length; i++) {
            GraphicsConfiguration conf = screens[i].getDefaultConfiguration();
            _frames[i] = new UiFrame(conf);
        }
    }
    
    
    public TextUI(int x, int y, int w, int h) {
        _frames = new UiFrame[1];
        _frames[0] = new UiFrame();
        _frames[0].setBounds(x, y, w, h);
        _frames[0].setVisible(true);
    }
    
    
    public void displayText(String s) {
        for(int i = _frames.length - 1; i >= 0; i--) {
            _frames[i].display(s);
        }
        (new ColorAnimator()).start();
    }
    
    
// --- STATIC METHODS --- //
    
    public static void main(String[] args) {
        
        TextUI ui;
        
        if(args.length == 4) {
            int x = Integer.parseInt(args[0]);
            int y = Integer.parseInt(args[1]);
            int w = Integer.parseInt(args[2]);
            int h = Integer.parseInt(args[3]);
            ui = new TextUI(x, y, w, h);
        } else {
            ui = new TextUI();
        }
        
        StringBuilder builder = new StringBuilder();
                
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
            
            if(ch == '\n') {
                ui.displayText(builder.toString());
                builder.setLength(0);
            } else {
                builder.append((char)ch);
            }
        }
    }
    
}
