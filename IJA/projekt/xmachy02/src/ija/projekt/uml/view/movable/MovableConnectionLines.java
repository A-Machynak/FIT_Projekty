/**
 * @file MovableConnectionArrows.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Arrow types
 *
 */

package ija.projekt.uml.view.movable;

public enum MovableConnectionLines {
    STRAIGHT,               // Simple line
    DIAMOND,                // Line which ends with a diamond shape
    FILLED_DIAMOND,         // Line which ends with a filled diamond shape
    TRIANGLE,               // Line which ends with a triangle shape
    DASHED,                 // Dashed line
    FILLED_TRIANGLE_MESSAGE // Line which ends with a filled triangle shape and contains a place for message
}
