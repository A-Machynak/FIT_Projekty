package ija.projekt.uml.view.movable;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;

public class MovableFocusOfControl extends MovableEntity {
    private static final int PADDING_Y_OFFSET = 5;
    private static final int CURSOR_OFFSET = 1;

    private static final int DEFAULT_WIDTH = 15;
    private static final int DEFAULT_HEIGHT = 30;

    private static final int MINIMUM_HEIGHT = 5;

    private int top;
    private int height;
    private boolean resizeTop = false;
    private boolean resizeBottom = false;

    protected int lastMousePosY = 0;

    public MovableFocusOfControl() {
        setForeground(Color.darkGray);
        setSize(DEFAULT_WIDTH, DEFAULT_HEIGHT + 2*PADDING_Y_OFFSET);
        //setBorder(BorderFactory.createDashedBorder(Color.red));

        top = PADDING_Y_OFFSET;
        height = DEFAULT_HEIGHT;

        addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseDragged(MouseEvent e) {
                super.mouseDragged(e);
                int y = e.getLocationOnScreen().y;
                int dy = y - lastMousePosY;
                var size = getSize();

                if(!resizeTop && !resizeBottom) { // move
                    if(top + dy <= 0) {
                        return;
                    }
                    top += dy;
                    setSize(size.width, size.height+dy);
                } else if(resizeTop) {
                    top += dy;
                    height -= dy;
                    if(top <= 0 || height <= MINIMUM_HEIGHT) {
                        top -= dy;
                        height += dy;
                    }
                } else { // resize bottom
                    height += dy;
                    if(height <= MINIMUM_HEIGHT) {
                        height = MINIMUM_HEIGHT;
                    } else {
                        setSize(size.width, size.height+dy);
                    }
                }

                lastMousePosY = y;
                repaint();
            }

            @Override
            public void mouseMoved(MouseEvent e) {
                super.mouseMoved(e);
                int h = e.getPoint().y;
                int bot = top + height;

                resizeBottom = false;
                resizeTop = false;

                if(h <= top+CURSOR_OFFSET && h >= top) {
                    resizeTop = true;
                    setCursor(Cursor.getPredefinedCursor(Cursor.N_RESIZE_CURSOR));
                } else {
                    if(h >= bot-CURSOR_OFFSET && h <= bot) {
                        resizeBottom = true;
                        setCursor(Cursor.getPredefinedCursor(Cursor.S_RESIZE_CURSOR));
                    }
                }
                lastMousePosY = e.getLocationOnScreen().y;

                if(!resizeBottom && !resizeTop) {
                    setCursor(Cursor.getDefaultCursor());
                }
            }
        });
    }

    @Override
    public void scale(float factor) {
        // Intentionally empty
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        var size= getSize();
        g.setColor(Color.LIGHT_GRAY);
        g.fillRect(0, top, size.width-1, height);
        g.setColor(Color.BLACK);
        g.drawRect(0, top, size.width-1, height);
    }

    /**
     * Overriden isMovable - only MovableLifeline should take care of moving this entity
     * @return false
     */
    @Override
    public boolean isMovable() {
        return false;
    }

    @Override
    public int getTop() {
        return top;
    }

    @Override
    public int getBottom() {
        return height + top;
    }
}
