/**
 * @file MovableDiamondArrow.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Arrow with a diamond shape at the end
 *
 */

package ija.projekt.uml.view.movable.line;

import ija.projekt.uml.utils.Pair;
import ija.projekt.uml.view.movable.MovableConnectionLines;

import java.awt.*;

public class MovableDiamondArrow extends MovableLine {
    protected static final float DIAMOND_SHAPE_ROTATION = 35f;
    protected Polygon arrowPolygon;

    public MovableDiamondArrow(Point startPos, Point endPos) {
        super(startPos, endPos);
        this.arrowType = MovableConnectionLines.DIAMOND;
        updateArrow();
    }

    @Override
    public void updateArrow() {
        super.updateArrow();
        arrowPolygon = getDiamondPolygon(DIAMOND_SHAPE_ROTATION);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        if(arrowType == MovableConnectionLines.DIAMOND) {
            Graphics2D g2d = (Graphics2D) g;
            g2d.setStroke(lineStroke);
            g2d.setColor(Color.black);

            Point startLocation = getStartPositionLocal();
            int firstX = arrowPolygon.xpoints[0];
            int firstY = arrowPolygon.ypoints[0];

            g2d.drawPolygon(arrowPolygon);
            g2d.drawLine(startLocation.x, startLocation.y, firstX, firstY);
        }
    }

    protected Polygon getDiamondPolygon(float rotation) {
        Point startLocation = getStartPositionLocal();
        Point endLocation = getEndPositionLocal();

        // Get the negative vector and normalize
        int vectX = (startLocation.x - endLocation.x);
        int vectY = (startLocation.y - endLocation.y);
        double length = Math.sqrt(vectX * vectX + vectY * vectY);
        double xNorm = vectX / length;
        double yNorm = vectY / length;
        double rad = Math.toRadians(rotation);

        // Rotate by +-[rotation] degrees to create an "arrow" shape
        Pair<int[], int[]> pair = new Pair<>(
                new int[] {
                        (int)((xNorm * Math.cos(rad) - yNorm * Math.sin(rad)) * ARROW_X_LENGTH),  // First rotated x coordinate
                        (int)((xNorm * Math.cos(-rad) - yNorm * Math.sin(-rad)) * ARROW_X_LENGTH) // Second rotated x coordinate
                },
                new int[] {
                        (int)((xNorm * Math.sin(rad) + yNorm * Math.cos(rad)) * ARROW_Y_LENGTH),  // First rotated y coordinate
                        (int)((xNorm * Math.sin(-rad) + yNorm * Math.cos(-rad)) * ARROW_Y_LENGTH) // Second rotated y coordinate
                }
        );

        // Project first rotated vector onto the negative normalized vector
        double A = ((pair.getFirst()[0] * xNorm + pair.getSecond()[0] * yNorm) /
                (xNorm * xNorm + yNorm * yNorm));
        int ax = (int) (A * xNorm * 2f);
        int ay = (int) (A * yNorm * 2f);

        // Create polylines (origin = ending position)
        int[] xPoints = new int[]{
                ax + endLocation.x,            // end of line (start of diamond)
                pair.getFirst()[0] + endLocation.x,  // first diamond vertex
                endLocation.x,                       // second diamond vertex (end of line)
                pair.getFirst()[1] + endLocation.x,  // third diamond vertex
                //ay + endPos.x
        };
        int[] yPoints = new int[] {
                ay + endLocation.y,
                pair.getSecond()[0] + endLocation.y,
                endLocation.y,
                pair.getSecond()[1] + endLocation.y,
                //ay + endPos.y
        };

        return new Polygon(xPoints, yPoints, xPoints.length);
    }
}
