/**
 * @file MovableArrowFactory.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Arrow factory
 *
 */

package ija.projekt.uml.view.movable.line;

import ija.projekt.uml.view.movable.MovableConnectionLines;

import java.awt.*;

/**
 * Abstract factory for MovableLine and it's subclasses
 */
public class MovableLineFactory {
    public static MovableLine create(Point p1, Point p2, MovableConnectionLines arrowType) {
        if(arrowType == MovableConnectionLines.DIAMOND) {
            return new MovableDiamondArrow(p1, p2);
        } else if(arrowType == MovableConnectionLines.FILLED_DIAMOND) {
            return new MovableFilledDiamondArrow(p1, p2);
        } else if(arrowType == MovableConnectionLines.TRIANGLE) {
            return new MovableTriangleArrow(p1, p2);
        } else if(arrowType == MovableConnectionLines.DASHED) {
            return new MovableDashedLine(p1, p2);
        } else {
            return new MovableLine(p1, p2);
        }
    }
}
