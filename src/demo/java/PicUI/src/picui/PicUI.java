/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package picui;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import javax.imageio.ImageIO;
import javax.swing.*;

/**
 *
 * @author hamed
 */
public class PicUI {

// --- NESTED TYPES --- //
    
    private static class ImageRecord {
        public int reference;
        public boolean view = false;
        public Rectangle cropRect;
        public Rectangle viewRect;
        public BufferedImage image;
    };
    
    
    private static class UiFrame extends JFrame {

    // --- FIELDS --- //
        
        private HashMap<Integer, ImageRecord> _images;
        private Rectangle _globalRect;
        private double _globalToLocalScaleX;
        private double _globalToLocalScaleY;
        private int _globalToLocalTranslateX;
        private int _globalToLocalTranslateY;
        
        
    // --- (DE)CONSTRUCTORS --- //
        
        public UiFrame(GraphicsConfiguration conf, Rectangle globalRect) {
            super(conf);
            setUndecorated(true);
            _globalRect = globalRect;
            init(conf.getBounds());
        }
        
        
        public UiFrame(Rectangle bounds, Rectangle globalRect) {
            _globalRect = globalRect;            
            init(bounds);
        }
        
        
        private void init(Rectangle bounds) {
            _images = new HashMap<Integer, ImageRecord>();
            
            _globalToLocalTranslateX = - _globalRect.x;
            _globalToLocalTranslateY = - _globalRect.y;
            
            _globalToLocalScaleX = _globalRect.getWidth() / bounds.getWidth();
            _globalToLocalScaleY = _globalRect.getHeight() / bounds.getHeight();

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
            
            setVisible(true);
        }

        
        public void load(int ref, BufferedImage image) {
            ImageRecord r = new ImageRecord();
            r.image = image;
            r.reference = ref;
            r.view = false;
            _images.put(ref, r);
        }
        
        
        public void put(int ref, Rectangle globalRect) {
            ImageRecord r = _images.get(ref);
            
            if(r == null) {
                return;
            }
            
            r.viewRect = new Rectangle(
                    (int)((globalRect.x + _globalToLocalTranslateX) * _globalToLocalScaleX),
                    (int)((globalRect.y + _globalToLocalTranslateY) * _globalToLocalScaleY), 
                    (int)(globalRect.width * _globalToLocalScaleX),
                    (int)(globalRect.height * _globalToLocalScaleY));
            
            double widthRatio = r.image.getWidth() / r.viewRect.getWidth();
            double heightRatio = r.image.getHeight() / r.viewRect.getHeight();
            
            if(widthRatio < heightRatio) {
                int h = (int)(r.viewRect.height * widthRatio);
                r.cropRect = new Rectangle(
                        0, (r.image.getHeight() - h)/2, r.image.getWidth(), h);
            } else {
                int w = (int)(r.viewRect.width * heightRatio);
                r.cropRect = new Rectangle(
                        (r.image.getWidth() - w)/2, 0, w, r.image.getHeight());
            }
            
            r.view = true;
            
            repaint();
        }
        
        
        public void unput(int ref) {
           ImageRecord r = _images.get(ref);
           if(r != null) {
               if(!r.view) {
                   _images.remove(ref);
               } else {
                   r.view = false;
               }
           }
            repaint();
        }
        
        
        @Override
        public void paint(Graphics g) {
            g.clearRect(0, 0, getWidth(), getHeight());
            for(ImageRecord r: _images.values()) {
                if(!r.view) {
                    continue;
                }
                g.drawImage(r.image, 
                        r.viewRect.x, r.viewRect.y, 
                        (int)r.viewRect.getMaxX(), (int)r.viewRect.getMaxY(),
                        r.cropRect.x, r.cropRect.y,
                        (int)r.cropRect.getMaxX(), (int)r.cropRect.getMaxY(),
                        rootPane);
            }
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
    
    Rectangle _virtualRect;
    Rectangle _physicalRect;
    UiFrame[] _frames;
    boolean _isWindowSetupDone = false;
    
    
// --- (DE)CONSTRUCTORS --- //
    
    PicUI() {    
        _frames = new UiFrame[0];
    }
    
    
    void setupFullScreen() {
        if(_isWindowSetupDone) {
            return;
        }
        
        GraphicsEnvironment gr = GraphicsEnvironment.getLocalGraphicsEnvironment();
        GraphicsDevice[] screens = gr.getScreenDevices();
        
        _frames = new UiFrame[screens.length];
        _physicalRect = new Rectangle();
        
        for(int i = 0; i < _frames.length; i++) {
            GraphicsConfiguration conf = screens[i].getDefaultConfiguration();
            _physicalRect = _physicalRect.union(conf.getBounds());
        }
        
        double physicalToVirtualFactorW = (double)_virtualRect.getWidth() / (double)_physicalRect.getWidth();
        double physicalToVirtualFactorH = (double)_virtualRect.getHeight() / (double)_virtualRect.getHeight();
        
        for(int i = 0; i < _frames.length; i++) {
            GraphicsConfiguration conf = screens[i].getDefaultConfiguration();
            Rectangle physicalRect = conf.getBounds();
            
            Rectangle virtualRect = new Rectangle(
                    _virtualRect.x + (int)(physicalRect.x * physicalToVirtualFactorW),
                    _virtualRect.y + (int)(physicalRect.y * physicalToVirtualFactorH),
                    (int)(_virtualRect.width * physicalToVirtualFactorW),
                    (int)(_virtualRect.height * physicalToVirtualFactorH));
                                
            _frames[i] = new UiFrame(conf, virtualRect);
        }
        
        _isWindowSetupDone = true;
    }
    
    
    void setupWindowed(Rectangle bounds) {
        if(_isWindowSetupDone) {
            return;
        }
        
        _frames = new UiFrame[1];
        _frames[0] = new UiFrame(bounds, _virtualRect);
        _physicalRect = bounds;
        
        _isWindowSetupDone = true;
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
            builder.append((char)ch);
        }
        
        int len = builder.length();        
        
        char[] chars = new char[len];
        builder.getChars(0, len, chars, 0);                        
        IntWrapper v = new IntWrapper();
        CharWrapper c = new CharWrapper();
        int pos = 0;
                
        pos = consumeSpaces(chars, len, pos);
        
        // QUIT command
        c.value = 'q';
        pos = readChar(chars, len, pos, c);
        if(!c.failed) {
            return false;
        }
        
        
        // WINDOW command
        
        c.value = 'w';
        pos = readChar(chars, len, pos, c);
        if(!c.failed) {
            // Read window origin

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int windowX = v.value;

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int windowY = v.value;


            // Read window virtual offset
            
            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int windowOffsetX = v.value;

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int windowOffsetY = v.value;

            
            // Read window size

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int windowWidth = v.value;

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int windowHeight = v.value;

            
            _virtualRect = new Rectangle(windowOffsetX, windowOffsetY, 
                    windowWidth, windowHeight);

            
            // Setup window

            if(windowX == - 1 || windowY == -1) {
                setupFullScreen();
            } else {
                Rectangle bounds = new Rectangle(windowX, windowY, 
                        windowWidth, windowHeight);
                
                setupWindowed(bounds);
            }
            
            return true;
        }

        
        // LOAD command
        
        c.value = 'l';
        pos = readChar(chars, len, pos, c);
        if(!c.failed) {
            
            // Read image reference
            
            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int ref = v.value;
            
            
            // Read file name

            pos = consumeSpaces(chars, len, pos);
            String fileName = builder.substring(pos, len);
        
            load(ref, fileName);
            
            return true;
        }
        
        // PUT command
        
        c.value='p';
        pos = readChar(chars, len, pos, c);
        if(!c.failed) {
            // Read image reference
            
            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int ref = v.value;
            
            
            // Read crop area

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int virtualOriginX = v.value;

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int virtualOriginY = v.value;

            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int virtualSizeW = v.value;
            
            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return true;
            }
            int virtualSizeH = v.value;
            
            put(ref, new Rectangle(virtualOriginX, virtualOriginY,
                    virtualSizeW, virtualSizeH));
            
            return true;
        }
        
        
        
        c.value = 'd';
        pos = readChar(chars, len, pos, c);
        if(!c.failed) {
            pos = readInt(chars, len, consumeSpaces(chars, len, pos), v);
            if(v.failed) {
                return false;
            }
            int ref = v.value;
            
            unput(ref);
            return true;
        }
        
        return true;
    }
    
    
    void put(int ref, Rectangle globalRect) {
        for(UiFrame f: _frames) {
            f.put(ref, globalRect);
        }
    }
    
    
    void load(int ref, String fileName) {
        try {
            BufferedImage img = ImageIO.read(new File(fileName));            
            for(UiFrame f: _frames) {
                f.load(ref, img);
            }
        } catch (IOException e) {
            // Nothing;
        }                
    }
    
    
    void unput(int ref) {
        for(UiFrame f: _frames) {
            f.unput(ref);
        }
    }

    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        PicUI ui = new PicUI();
        
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
        
        System.exit(0);
    }
    
}
