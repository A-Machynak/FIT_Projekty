/**
 * @file MovableTriangleArrow.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Arrow with a triangle shape at the end
 *
 */

package ija.projekt.uml.view.movable.line;

import ija.projekt.uml.view.movable.MovableConnectionLines;

import java.awt.*;

public class MovableTriangleArrow extends MovableDiamondArrow {
    protected static final float TRIANGLE_SHAPE_ROTATION = 45f;

    public MovableTriangleArrow(Point startPos, Point endPos) {
        super(startPos, endPos);
    }

    @Override
    public void updateArrow() {
        Point endLocation = getEndPositionLocal();

        arrowPolygon = getDiamondPolygon(TRIANGLE_SHAPE_ROTATION);

        // Divide first coordinate by 2 to get a triangle shape from diamond shape
        int firstX = arrowPolygon.xpoints[0];
        int firstY = arrowPolygon.ypoints[0];

        // move back to (0,0), divide and then back to correct coordinates
        arrowPolygon.xpoints[0] = ((firstX - endLocation.x) / 2) + endLocation.x;
        arrowPolygon.ypoints[0] = ((firstY - endLocation.y) / 2) + endLocation.y;
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        if(arrowType == MovableConnectionLines.TRIANGLE) {
            Graphics2D g2d = (Graphics2D) g;
            g2d.setStroke(lineStroke);
            g2d.setColor(Color.black);

            Point startLocation = getStartPosition();
            int firstX = arrowPolygon.xpoints[0];
            int firstY = arrowPolygon.ypoints[0];

            g2d.drawPolygon(arrowPolygon);
            g2d.drawLine(startLocation.x, startLocation.y, firstX, firstY);
        }
    }
}
