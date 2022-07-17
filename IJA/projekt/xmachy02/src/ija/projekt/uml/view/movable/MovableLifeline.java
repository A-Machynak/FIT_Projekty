package ija.projekt.uml.view.movable;

import javax.swing.*;
import javax.swing.event.EventListenerList;
import javax.swing.event.MouseInputListener;
import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import java.util.Collections;

/**
 * Lifeline is made out of a rectangle and a dashed line
 */
public class MovableLifeline extends MovableEntity implements MouseInputListener {
    protected MovableRectangle rectangle;

    protected BasicStroke lineStroke;
    protected ArrayList<MovableFocusOfControl> focs = new ArrayList<>();

    protected static final int LINE_STROKE_WIDTH = 1;
    protected static final int CURSOR_OFFSET = 5;

    protected int lifeTime = 200;
    protected int LIFETIME_OFFSET = 20;

    protected int lastMousePosY = 0;

    protected final EventListenerList listenerList = new EventListenerList();

    public MovableLifeline(Point position, String name) {
        super(position);
        lineStroke = new BasicStroke(LINE_STROKE_WIDTH,
                BasicStroke.CAP_BUTT,
                BasicStroke.JOIN_MITER,
                1.0f,
                new float[]{ 10.0f, 4.0f },
                0.0f);
        rectangle = new MovableRectangle();

        setLayout(new BorderLayout());
        setComponentZOrder(rectangle, 0);

        rectangle.setTopText(name);
        rectangle.showMidText(false);
        rectangle.showBotText(false);
        //setBorder(BorderFactory.createDashedBorder(Color.red));

        add(rectangle, BorderLayout.PAGE_START);
        addMouseListener(this);
        addMouseMotionListener(this);
    }

    public void setName(String name) {
        rectangle.setTopText(name);
    }

    public java.util.List<MovableFocusOfControl> getFOCs() {
        return Collections.unmodifiableList(focs);
    }

    /**
     * Add focus of control to this lifeline
     * @param foc focus of control
     */
    public void bindFOC(MovableFocusOfControl foc) {
        foc.setPosition(position);
        focs.add(foc);
    }

    /**
     * Update all of the focs (called after changing locations)
     */
    private void updateFOCsPos() {
        var loc = getLocation();
        var height = getSize().height;
        var newX = getSize().width/2 + loc.x;
        var newY = height-lifeTime-LIFETIME_OFFSET;

        for(MovableFocusOfControl mfoc : focs) {
            var y = loc.y + newY;
            mfoc.setLocation(newX - mfoc.getSize().width/2, y);
        }
    }

    @Override
    public void setLocation(int x, int y) {
        super.setLocation(x, y);
        updateFOCsPos();
    }

    @Override
    public void scale(float factor) {
        // Intentionally empty
    }

    @Override
    public void setSize(int x, int y) {
        super.setSize(x, y + lifeTime + LIFETIME_OFFSET);
        //updateArrow();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;

        g2d.setStroke(lineStroke);
        g2d.setColor(Color.black);
        var size = rectangle.getSize();
        g2d.drawLine(size.width/2, size.height, size.width/2, size.height + lifeTime);
    }

    @Override
    public int getTop() {
        return LIFETIME_OFFSET;
    }

    @Override
    public int getLeft() {
        return rectangle.getSize().width/2;
    }

    @Override
    public int getRight() {
        return rectangle.getSize().width/2;
    }

    /**
     * Add a listener
     * @param listener listener to add
     */
    public void addListener(ActionListener listener) {
        listenerList.add(ActionListener.class, listener);
    }

    private void callListeners(ActionEvent e) {
        for(ActionListener l : listenerList.getListeners(ActionListener.class)) {
            l.actionPerformed(e);
        }
    }

    @Override
    public void mouseClicked(MouseEvent e) {
        if(e.getButton() == MouseEvent.BUTTON3) {
            callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "delete"));
        }
    }

    @Override
    public void mousePressed(MouseEvent e) {

    }

    @Override
    public void mouseReleased(MouseEvent e) {

    }

    @Override
    public void mouseEntered(MouseEvent e) {

    }

    @Override
    public void mouseExited(MouseEvent e) {

    }

    @Override
    public void mouseDragged(MouseEvent e) {
        if(!movable) {
            var pos = getPosition();
            var size = getSize();
            int y = e.getLocationOnScreen().y;
            int dy = y - lastMousePosY;

            lifeTime += dy;
            lastMousePosY = y;

            // ! update the height and position
            setSize(size.width, dy);
            //setPosition(pos.x, pos.y + dy);

            repaint();
        }
    }

    @Override
    public void mouseMoved(MouseEvent e) {
        int h = e.getPoint().y;
        int totalH = getHeight();

        if(h >= totalH - LIFETIME_OFFSET - CURSOR_OFFSET && h <= totalH - LIFETIME_OFFSET + CURSOR_OFFSET) {
            setCursor(Cursor.getPredefinedCursor(Cursor.S_RESIZE_CURSOR));
            movable = false;
            lastMousePosY = e.getLocationOnScreen().y;
        } else {
            setCursor(Cursor.getDefaultCursor());
            movable = true;
        }
    }
}
