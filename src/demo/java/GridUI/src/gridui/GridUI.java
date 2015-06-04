/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package gridui;

import java.awt.*;
import java.awt.event.*;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Vector;
import javax.swing.*;

/**
 *
 * @author hamed
 */
public class GridUI {
    
// --- NESTED TYPES --- //
    
    private static class UiFrame extends JFrame {
        
    // --- FIELDS --- //
        
        private SimpleGridCanvas _gridCanvas;
        private Rectangle _gridBounds;
     
        
    // --- (DE)CONSTRUCTORS --- //
        
        public UiFrame(GraphicsConfiguration conf) {
            super(conf);
            setUndecorated(true);
            init(conf.getBounds());
        }
        
        
        public UiFrame(Rectangle bounds) {
            init(bounds);
        }
        
        
        private void init(Rectangle bounds) {
            KeyListener keyListener = new KeyListener() {

                @Override
                public void keyTyped(KeyEvent e) {
                    if(e.getKeyChar() == 'X') {
                        System.exit(0);
                    }
                }

                @Override
                public void keyPressed(KeyEvent e) {
                    // Nothing;
                }

                @Override
                public void keyReleased(KeyEvent e) {
                    // Nothing
                }
            };
            
            addKeyListener(keyListener);
            
            setBounds(bounds);
            setPreferredSize(bounds.getSize());
            setLayout(new BorderLayout());
            
            _gridCanvas = new SimpleGridCanvas();
            _gridCanvas.setCellSpacing(1);
            _gridCanvas.showGrid();
        
            add(_gridCanvas, BorderLayout.CENTER);
            
            setVisible(true);
        }
        
        
        void setGridBounds(Rectangle bounds) {
            _gridBounds = bounds;
            _gridCanvas.setGridSize(bounds.width, bounds.height);
        }
    
    
        void testAndSet(int x, int y, Color c) {
            if(!_gridBounds.contains(x, y)) {
                return;
            }
            
            _gridCanvas.setCellColor(x - _gridBounds.x, y - _gridBounds.y, c);
        }
        
    };
    
    
    private static class IntWrapper {
        public int value;        
        public boolean failed;
    };
    
    
    private static class CharWrapper {
        public int value;
        public boolean failed;
    };
    
            
// --- STATIC METHODS --- //
        
    private static int consumeSpaces(char[] chars, int nchars, int pos) {
        while(pos < nchars && (chars[pos] == ' ' || chars[pos] == '\n')) {
            pos++;
        }
        return pos;
    }
    
    
    private static int readInt(char[] chars, int nchars, int pos, IntWrapper result) {
        if(pos >= nchars) {
            result.failed = true;
            return nchars;
        }
        
        int signBegin = pos;
        if(chars[pos] == '-' || chars[pos] == '+') {
            pos++;
        }
        
        if(pos >= nchars) {
            result.failed = true;
            return signBegin;
        }
        
        int begin = pos;
        while(pos < nchars && Character.isDigit(chars[pos])) {
            pos++;
        }
        
        if(pos == begin) {
            result.failed = true;
            return signBegin;
        }
        
        String intStr = new String(chars, signBegin, pos - signBegin);        
        result.value = Integer.parseInt(intStr);
        result.failed = false;
                        
        return pos;
    }
    
    
    private static int readChar(char[] chars, int nChars, int pos, CharWrapper result) {
        if(pos >= nChars) {
            result.failed = true;
            return nChars;
        }
        
        if(chars[pos] == result.value) {
            result.failed = false;
            return pos + 1;
        }
        
        result.failed = true;
        return pos;
    }

    
// --- FIELDS --- //
    
    UiFrame[] _frames;
    boolean _isWindowSetupDone;
    boolean _gridSetupDone = false;
    
    
// --- (DE)CONSTRUCTORS --- //
    
    GridUI() {    
        _isWindowSetupDone = false;
    }
    
    
// --- METHODS --- //    
    
    void setupFullScreen() {
        if(_isWindowSetupDone) {
            return;
        }
        
        GraphicsEnvironment gr = GraphicsEnvironment.getLocalGraphicsEnvironment();
        GraphicsDevice[] screens = gr.getScreenDevices();
        
        // _frames = new UiFrame[screens.length];
        
        ArrayList<UiFrame> v = new ArrayList<UiFrame>();
        
        for(int i = 0; i < screens.length; i++) {
            GraphicsConfiguration conf = screens[i].getDefaultConfiguration();
            boolean isNew = true;
            
            for(int j = 0; j < i; j++) {
                if(!conf.getBounds().intersection(_frames[j].getBounds()).isEmpty()) {
                    isNew = false;
                }
            }
            
            if(isNew) {
                v.add(new UiFrame(conf));
            }
        }
        
        _frames = v.toArray(_frames);
        
        Comparator<UiFrame> c = new Comparator() {
            @Override
            public int compare(Object o1, Object o2) {
                return Integer.compare(((UiFrame)o1).getX(), ((UiFrame)o2).getX());
            }
        };
        
        java.util.Arrays.sort(_frames, c);
        
        _isWindowSetupDone = true;
    }
    
    
    void setupWindowed(Rectangle bounds) {
        if(_isWindowSetupDone) {
            return;
        }
        
        _frames = new UiFrame[1];
        _frames[0] = new UiFrame(bounds);
        
        _isWindowSetupDone = true;
    }

    
    void setGridSize(int w, int h) {                
        int widthPerFrame = w/_frames.length;
        int remainder = w%_frames.length;
        
        int left = 0;
        int width = 0;
        for(int i = 0; i < _frames.length; i++) {
            width = widthPerFrame;
            if(i == 0) {
                width += remainder;
            }
            
            Rectangle bounds = new Rectangle(left, 0, width, h);
            _frames[i].setGridBounds(bounds);
            
            left += width;
        }
    }
    
    
    void setCell(int x, int y, Color c) {
        for(int i = _frames.length - 1; i >= 0; i--) {
           _frames[i].testAndSet(x, y, c);
        }
    }
    
    
    boolean readFromStream(InputStream in) throws IOException {
        StringBuilder builder = new StringBuilder();
        
        while(true) {
            int ch = in.read();
            if(ch == -1) {
                return false;
            }
            if(ch == '#') {
                break;
            }
            if(ch == 'q') {
                GraphicsEnvironment.getLocalGraphicsEnvironment()
                        .getDefaultScreenDevice().setFullScreenWindow(null);
                return false;
            }
            builder.append((char)ch);
        }
        
        int len = builder.length();        
        
        char[] chars = new char[len];
        builder.getChars(0, len, chars, 0);                        
        IntWrapper v = new IntWrapper();
        CharWrapper c = new CharWrapper();
        int pos = 0;
        
        c.value = 'w';
        pos = consumeSpaces(chars, len, pos);
        pos = readChar(chars, len, pos, c);
        if(!c.failed) {
            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return false;
            }
            int windowX = v.value;

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return false;
            }
            int windowY = v.value;

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return false;
            }
            int windowWidth = v.value;

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return false;
            }
            int windowHeight = v.value;
            
            setupWindowed(new Rectangle(windowX, windowY, windowWidth, windowHeight));
        }

        c.value = 'f';
        pos = consumeSpaces(chars, len, pos);
        pos = readChar(chars, len, pos, c);
        if(!c.failed) {
            setupFullScreen();
        }
                        
        pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
        if(v.failed) {
            return false;
        }
        int gridWidth = v.value;
        
        pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
        if(v.failed) {
            return false;
        }
        int gridHeight = v.value;
        
        
        if(!_gridSetupDone) {
            setGridSize(gridWidth, gridHeight);
            _gridSetupDone = true;
        }
                        
        pos = consumeSpaces(chars, len, pos);
        
        int x = 0;
        int y = 0;        
                
        while(pos < len) {
            // r
            c.value = 'r';
            pos = readChar(chars, len, pos, c);
            if(c.failed) {
                return false;
            }
            
            pos = readInt(chars, len, pos, v);
            if(v.failed) {
                return false;
            }
            int r = v.value;
            
            // g
            c.value = 'g';
            pos = readChar(chars, len, pos, c);
            if(c.failed) {
                return false;
            }
            
            pos = readInt(chars, len, pos, v);
            if(v.failed) {
                return false;
            }
            int g = v.value;
            
            // b
            c.value = 'b';
            pos = readChar(chars, len, pos, c);
            if(c.failed) {
                return false;
            }
            
            pos = readInt(chars, len, pos, v);
            if(v.failed) {
                return false;
            }
            int b = v.value;
            
           Color color = new Color(r, g, b);
           setCell(x, y, color);
           
           x++;
           if(x >= gridWidth) {
               x = 0;
               y++;
           }
           
           if(y >= gridHeight) {
               y = 0;
               x = 0;
           }
        }
        
        for(UiFrame f: _frames) {
            f.repaint();
        }
        
        return true;
    }
    

// --- MAIN --- //
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        GridUI ui = new GridUI();
        
        InputStream is = System.in;
        
        if(args.length == 1) {
            try {
                is = new FileInputStream(args[0]);
            } catch(FileNotFoundException e) {
                System.out.println(e);
                return;
            }
        }
        
        try {
            while(ui.readFromStream(is)) {
                // Nothing;
            }
        } catch(IOException e) {
            System.out.println(e);
        }
    }

}
