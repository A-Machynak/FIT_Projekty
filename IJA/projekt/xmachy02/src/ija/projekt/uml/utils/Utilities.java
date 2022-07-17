/**
 * @file Utilities.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Utility functions
 *
 */

package ija.projekt.uml.utils;

import java.awt.*;

public class Utilities {

    /**
     * Checks whether point is in a rectangle and returns which part of rectangle the point is in (direction from middle).
     * @param p point to check
     * @param topLeft top left point of the rectangle
     * @param bottomRight top right point of the rectangle
     * @return Which part of rectangle the point is in (direction from middle).
     * Returns Directions.NONE if the point isn't in a rectangle
     */
    public static Directions getPointInRectangle(Point p, Point topLeft, Point bottomRight) {
        int width = bottomRight.x - topLeft.x;
        int height = bottomRight.y - topLeft.y;

        float width13 = width / 3f;
        float height13 = height / 3f;

        if(p.x > bottomRight.x || p.x < topLeft.x || p.y > bottomRight.y || p.y < topLeft.y) {
            return Directions.NONE;
        }

        if(p.x < (topLeft.x + width13)) {
            // LEFT
            if(p.y < (topLeft.y + height13)) {
                // TOP
                return Directions.TOP_LEFT;
            } else if(p.y < (topLeft.y + (2 * height13))) {
                // MIDDLE
                return Directions.TOP_MID;
            } else {
                // RIGHT
                return Directions.TOP_RIGHT;
            }
        } else if(p.x < (topLeft.x + (2 * width13))) {
            // MIDDLE
            if(p.y < (topLeft.y + height / 2)) {
                // LEFT
                return Directions.MID_LEFT;
            } else {
                // RIGHT
                return Directions.MID_RIGHT;
            }
        } else {
            // RIGHT
            if(p.y < (topLeft.y + height13)) {
                // TOP
                return Directions.BOT_LEFT;
            } else if(p.y < (topLeft.y + (2 * height13))) {
                // MIDDLE
                return Directions.BOT_MID;
            } else {
                // RIGHT
                return Directions.BOT_RIGHT;
            }
        }
    }

    /**
     * Get rectangle which is bounded by 2 points
     * @param p1 first bounding point
     * @param p2 second bounding point
     * @return Bounding rectangle
     */
    public static Rectangle getBoundingRectangle(Point p1, Point p2) {
        Rectangle rect = new Rectangle();
        rect.width = Math.abs(p1.x - p2.x);
        rect.height = Math.abs(p1.y - p2.y);

        rect.x = Math.min(p1.x, p2.x);
        rect.y = Math.min(p1.y, p2.y);

        return rect;
    }
}
