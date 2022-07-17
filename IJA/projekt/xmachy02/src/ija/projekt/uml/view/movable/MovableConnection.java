/**
 * @file MovableConnection.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Connection between 2 entities displayed using an arrow
 *
 */

package ija.projekt.uml.view.movable;

import ija.projekt.uml.view.movable.line.MovableLine;
import ija.projekt.uml.view.movable.line.MovableLineFactory;

import java.awt.*;

public class MovableConnection {
    private MovableLine arrow;
    private MovableEntity first;
    private MovableEntity second;

    public MovableConnection(MovableEntity first, MovableEntity second, MovableConnectionLines arrowType) {
        this.first = first;
        this.second = second;
        this.arrow = MovableLineFactory.create(first.getPosition(), second.getPosition(), arrowType);
        recalculateArrow();
    }

    public void recalculateArrow() {
        Point firstCenter = first.getPosition();
        Dimension firstSize = first.getSize();

        Point secondCenter = second.getPosition();
        Dimension secondSize = second.getSize();

        // Figure out, where the containers are relative to each other
        Point firstFinal = new Point(firstCenter.x, firstCenter.y);
        Point secondFinal = new Point(secondCenter.x, secondCenter.y);

        if(firstCenter.x > secondCenter.x) {
            if(firstCenter.x - firstSize.width/2 > secondCenter.x) {
                firstFinal.x = firstCenter.x - firstSize.width/2;
                secondFinal.x = secondCenter.x + secondSize.width/2;
            }
        } else if(firstCenter.x < secondCenter.x) {
            if(firstCenter.x - firstSize.width/2 <  secondCenter.x) {
                firstFinal.x = firstCenter.x + firstSize.width/2;
                secondFinal.x = secondCenter.x - secondSize.width/2;
            }
        }

        if(firstCenter.y > secondCenter.y) {
            if(firstCenter.y - firstSize.height/2 > secondCenter.y) {
                firstFinal.y = firstCenter.y - firstSize.height/2;
                secondFinal.y = secondCenter.y + secondSize.height/2;
            }
        } else if(firstCenter.y < secondCenter.y) {
            if(firstCenter.y - firstSize.height/2 < secondCenter.y) {
                firstFinal.y = firstCenter.y + firstSize.height/2;
                secondFinal.y = secondCenter.y - secondSize.height/2;
            }
        }

        this.arrow.setStartPosition(firstFinal);
        this.arrow.setEndPosition(secondFinal);
    }

    public MovableLine getArrow() {
        return arrow;
    }

    public void setArrow(MovableLine arrow) {
        this.arrow = arrow;
    }

    public MovableEntity getFirst() {
        return first;
    }

    public void setFirst(MovableEntity first) {
        this.first = first;
    }

    public MovableEntity getSecond() {
        return second;
    }

    public void setSecond(MovableEntity second) {
        this.second = second;
    }
}
