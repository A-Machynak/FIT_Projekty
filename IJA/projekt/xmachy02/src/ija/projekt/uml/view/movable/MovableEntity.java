/**
 * @file MovableEntity.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Abstract movable entity used in MovableCanvas
 *
 */

package ija.projekt.uml.view.movable;

import ija.projekt.uml.utils.Pair;

import javax.swing.*;
import java.awt.*;
import java.io.Serializable;

public abstract class MovableEntity extends JPanel implements Serializable, MovableBounds {
    protected Point position;
    protected boolean movable = true;

    public MovableEntity() {
        this(new Point(0, 0));
    }

    public MovableEntity(int xPos, int yPos) {
        this(new Point(xPos, yPos));
    }

    public MovableEntity(Point position) {
        this.position = position;
        setOpaque(false);
    }

    /**
     * Position getter
     * @return current position
     */
    public Point getPosition() {
        return (Point) position.clone();
    }

    /**
     * Position setter
     * @param position new position
     */
    public void setPosition(Point position) {
        this.position = position;
    }
    /**
     * Position setter
     * @param x new x position
     * @param y new y position
     */
    public void setPosition(int x, int y) {
        this.position.x = x;
        this.position.y = y;
    }

    /**
     * Indicates whether or not should this entity be moved
     * @return true if this entity can be moved
     */
    public boolean isMovable() {
        return movable;
    }

    /**
     * Scale container's content by a certain factor
     * @param factor scaling factor
     */
    public abstract void scale(float factor);

    /**
     * Get bounding points
     * @return pair of 2 points representing top left and bottom right coordinates
     */
    public Pair<Point, Point> getBoundingPoints() {
        Dimension size = getSize();
        return new Pair<>(
                new Point(position.x, position.y),
                new Point(position.x + size.width, position.y + size.height));
    }

    /**
     * Get bounding rectangle using the container's coordinates
     * @return pair of 2 points representing top left and bottom right coordinates
     */
    public Pair<Point, Point> getContainerLocBoundingRectangle() {
        Dimension size = getSize();
        Point cPos = getLocation();

        return new Pair<>(
                new Point(cPos.x, cPos.y),
                new Point(cPos.x + size.width, cPos.y + size.height));
    }

    @Override
    public int getTop() {
        return 0;
    }

    @Override
    public int getBottom() {
        return getSize().height;
    }

    @Override
    public int getLeft() {
        return 0;
    }

    @Override
    public int getRight() {
        return getSize().width;
    }
}
