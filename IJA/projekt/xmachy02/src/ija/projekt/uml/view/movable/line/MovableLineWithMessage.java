package ija.projekt.uml.view.movable.line;

import ija.projekt.uml.view.movable.MovableEntity;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import java.awt.geom.Rectangle2D;

/**
 * Draws a line between bounds of two entities.
 */
public class MovableLineWithMessage extends MovableEntity {
    public enum LineType {
        NORMAL_TRIANGLE,    // Normal line with filled triangle arrowhead
        DASHED_NORMAL,      // Dashed line with normal arrowhead
        NORMAL              // Normal line with normal arrowhead
    }

    protected final MovableEntity c1;
    protected final MovableEntity c2;

    private static final int DEFAULT_HEIGHT = 30;
    private static final int TRIANGLE_SIZE = 8;

    protected String message = "";
    protected BasicStroke lineStroke;
    protected BasicStroke normalStroke;
    protected LineType lineType;

    public MovableLineWithMessage(MovableEntity c1, MovableEntity c2, LineType lineType) {
        this.c1 = c1;
        this.c2 = c2;
        this.lineType = lineType;

        if(lineType.equals(LineType.NORMAL) || lineType.equals(LineType.NORMAL_TRIANGLE)) {
            this.lineStroke = new BasicStroke(2.f);
        } else {
            this.lineStroke = new BasicStroke(2.f, BasicStroke.CAP_BUTT, BasicStroke.JOIN_MITER, 10.0f, new float[]{10.0f}, 0.0f);
        }
        this.normalStroke = new BasicStroke(2.f);

        var height = Math.abs(c1.getTop() - c2.getBottom());

        setSize(getWidth(), DEFAULT_HEIGHT);
        setPosition(c1.getLeft(), height/2);
        //setBorder(BorderFactory.createDashedBorder(Color.BLUE));

        addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseDragged(MouseEvent e) {
                super.mouseDragged(e);
            }

            @Override
            public void mouseMoved(MouseEvent e) {
                super.mouseMoved(e);
            }
        });
    }

    public void update() {

    }

    public void setMessage(String message) {
        this.message = message;
        repaint();
    }

    public String getMessage() {
        return message;
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;

        int startX, endX, h = DEFAULT_HEIGHT - DEFAULT_HEIGHT/3;
        if(c1.getLocation().x > c2.getLocation().x) {
            startX = getWidth();
            endX = 0;
        } else {
            startX = 0;
            endX = getWidth();
        }

        g2d.setStroke(normalStroke);
        if(lineType.equals(LineType.NORMAL) || lineType.equals(LineType.DASHED_NORMAL)) {
            drawNormal(g2d, startX, endX, h);
        } else if(lineType.equals(LineType.NORMAL_TRIANGLE)) {
            drawWithTriangle(g2d, startX, endX, h);
        }

        // draw centered text above line
        /// Source: https://stackoverflow.com/questions/5378052/positioning-string-in-graphic-java
        FontMetrics fm = g.getFontMetrics();
        Rectangle2D rect = fm.getStringBounds(message, g);
        g2d.drawString(message, (int) (getWidth()/2 - rect.getWidth()/2), (int) (DEFAULT_HEIGHT/3 + rect.getHeight()/2));

        //g2d.drawString(message, getWidth()/2, DEFAULT_HEIGHT/3);

        // draw line
        g2d.setStroke(lineStroke);
        g2d.setColor(Color.black);
        g2d.drawLine(startX, h, endX, h);
    }

    public void drawNormal(Graphics2D g2d, int startX, int endX, int y) {
        if(endX > startX) {
            g2d.drawLine(endX, y, endX-TRIANGLE_SIZE, y-TRIANGLE_SIZE);
            g2d.drawLine(endX, y, endX-TRIANGLE_SIZE, y+TRIANGLE_SIZE);
        } else {
            g2d.drawLine(endX, y, endX+TRIANGLE_SIZE, y-TRIANGLE_SIZE);
            g2d.drawLine(endX, y, endX+TRIANGLE_SIZE, y+TRIANGLE_SIZE);
        }
    }

    public void drawWithTriangle(Graphics2D g2d, int startX, int endX, int y) {
        Polygon p = new Polygon();
        if(endX > startX) {
            p.addPoint(endX, y);
            p.addPoint(endX - TRIANGLE_SIZE, y + TRIANGLE_SIZE);
            p.addPoint(endX - TRIANGLE_SIZE, y - TRIANGLE_SIZE);
            p.addPoint(endX, y);
        } else {
            p.addPoint(0, y);
            p.addPoint(TRIANGLE_SIZE, y+TRIANGLE_SIZE);
            p.addPoint(TRIANGLE_SIZE, y-TRIANGLE_SIZE);
            p.addPoint(0, y);
        }
        g2d.fillPolygon(p);
    }

    public void drawString(Graphics2D g2d, int x, int y) {
        g2d.drawString(message, x, y);
    }

    @Override
    public void scale(float factor) {
        // Intentionally empty
    }

    @Override
    public void setSize(int width, int height) {
        super.setSize(getWidth(), DEFAULT_HEIGHT);
    }

    @Override
    public void setLocation(int x, int y) {
        Point c1Loc = c1.getLocation(), c2Loc = c2.getLocation();
        if(c1Loc.x > c2Loc.x) {
            x = c2Loc.x + c2.getRight();
        } else {
            x = c1Loc.x + c1.getRight();
        }

        super.setLocation(x, y);

    }

    @Override
    public int getWidth() {
        Point c1Loc = c1.getLocation(), c2Loc = c2.getLocation();
        if(c1Loc.x > c2Loc.x) {
            return Math.abs(c1Loc.x + c1.getRight() - (c2Loc.x + c1.getRight() + c2.getRight() - c1.getLeft()));
        } else {
            return Math.abs(c2Loc.x + c2.getRight() - (c1Loc.x + c1.getRight() + c2.getRight() - c1.getLeft()));
        }
    }
}
