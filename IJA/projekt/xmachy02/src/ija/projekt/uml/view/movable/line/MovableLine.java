/**
 * @file MovableArrow.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Basic arrow view (straight line)
 *
 */

package ija.projekt.uml.view.movable.line;

import ija.projekt.uml.utils.Utilities;
import ija.projekt.uml.view.movable.MovableConnectionLines;
import ija.projekt.uml.view.movable.MovableEntity;

import java.awt.*;

/**
 * Draws a line between start and end positions. Dynamically resizes the bounding box
 */
public class MovableLine extends MovableEntity {
    /**
     * Starting position
     */
    protected Point startPosition;
    /**
     * Ending position
     */
    protected Point endPosition;
    /**
     * Local starting position
     */
    protected Point startPositionLocal;
    /**
     * Local ending position
     */
    protected Point endPositionLocal;

    protected Rectangle boundingRectangle;

    protected static final int ARROW_X_LENGTH = 15;
    protected static final int ARROW_Y_LENGTH = 15;
    protected static final int HEIGHT_OFFSET = 5;
    protected static final int WIDTH_OFFSET = 5;
    protected static final int LINE_STROKE_WIDTH = 2;

    protected BasicStroke lineStroke;

    protected MovableConnectionLines arrowType = MovableConnectionLines.STRAIGHT;

    @Override
    public void scale(float factor) {
        // Intentionally empty
    }

    public MovableLine(Point startPos, Point endPos) {
        this.startPosition = startPos;
        this.endPosition = endPos;

        this.lineStroke = new BasicStroke(LINE_STROKE_WIDTH);
        this.startPositionLocal = new Point(0, 0);
        this.endPositionLocal = new Point(0, 0);

        setLayout(null);
        setForeground(Color.black);
        setOpaque(false);

        recalculateContainer();
    }

    /**
     * Update arrow (after changing position)
     */
    public void updateArrow() {
        recalculateContainer();
    }

    /**
     * Recalculate container's position and size using arrow's startPosition and endPosition.
     * Called after setStartPosition() or setEndPosition() is called.
     */
    private void recalculateContainer() {
        boundingRectangle = Utilities.getBoundingRectangle(startPosition, endPosition);

        setPosition(new Point(boundingRectangle.x, boundingRectangle.y));
        setSize(boundingRectangle.width + WIDTH_OFFSET, boundingRectangle.height + HEIGHT_OFFSET);
        updateLocalCoordinates();
    }

    /**
     * Recalculate line start and end positions using position.
     * Called after setPosition() is called.
     */
    private void recalculatePositions() {
        int dX = boundingRectangle.x - position.x;
        int dY = boundingRectangle.y - position.y;

        // move by delta (old and new position)
        startPosition.x += dX;
        startPosition.y += dY;
        endPosition.x += dX;
        endPosition.y += dY;

        // recalculate the bounding rectangle
        boundingRectangle = Utilities.getBoundingRectangle(startPosition, endPosition);
    }

    private void updateLocalCoordinates() {
        this.startPositionLocal.x = startPosition.x - boundingRectangle.x;
        this.startPositionLocal.y = startPosition.y - boundingRectangle.y;
        this.endPositionLocal.x = endPosition.x - boundingRectangle.x + WIDTH_OFFSET/2;
        this.endPositionLocal.y = endPosition.y - boundingRectangle.y + HEIGHT_OFFSET/2;
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        if(arrowType == MovableConnectionLines.STRAIGHT) {
            Graphics2D g2d = (Graphics2D) g;

            g2d.setStroke(lineStroke);
            g2d.setColor(Color.black);
            g2d.drawLine(startPositionLocal.x, startPositionLocal.y, endPositionLocal.x, endPositionLocal.y);
        }
    }

    @Override
    public void setPosition(Point p) {
        super.setPosition(p);
        recalculatePositions();
    }

    @Override
    public void setPosition(int x, int y) {
        super.setPosition(x, y);
        recalculatePositions();
    }

    public Point getStartPosition() { return (Point) startPosition.clone(); }

    public void setStartPosition(Point startPosition) {
        this.startPosition = startPosition;
        recalculateContainer();
    }

    public Point getEndPosition() { return (Point) endPosition.clone(); }

    public void setEndPosition(Point endPosition) {
        this.endPosition = endPosition;

        recalculateContainer();
    }

    public Point getStartPositionLocal() { return (Point) startPositionLocal.clone(); }

    public void setStartPositionLocal(Point startPositionLocal) { this.startPositionLocal = startPositionLocal; }

    public Point getEndPositionLocal() { return (Point) endPositionLocal.clone(); }

    public void setEndPositionLocal(Point endPositionLocal) { this.endPositionLocal = endPositionLocal; }

    public MovableConnectionLines getArrowType() { return arrowType; }

    public void setLineStrokeWidth(int width) { lineStroke = new BasicStroke(width); }

    @Override
    public void setLocation(Point p) {
        // Intentionally empty
    }

    @Override
    public void setLocation(int x, int y) {
        super.setLocation(x, y);
    }
}
