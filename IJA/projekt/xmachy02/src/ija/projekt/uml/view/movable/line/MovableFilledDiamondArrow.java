/**
 * @file MovableFilledDiamondArrow.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Arrow with a filled diamond shape at the end
 *
 */

package ija.projekt.uml.view.movable.line;

import ija.projekt.uml.view.movable.MovableConnectionLines;

import java.awt.*;

public class MovableFilledDiamondArrow extends MovableDiamondArrow {

    public MovableFilledDiamondArrow(Point startPos, Point endPos) {
        super(startPos, endPos);
        this.arrowType = MovableConnectionLines.FILLED_DIAMOND;
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        if(arrowType == MovableConnectionLines.FILLED_DIAMOND) {
            Graphics2D g2d = (Graphics2D) g;
            g2d.setStroke(lineStroke);
            g2d.setColor(Color.black);

            Point startLocation = getStartPositionLocal();
            int firstX = arrowPolygon.xpoints[0];
            int firstY = arrowPolygon.ypoints[0];

            g2d.fillPolygon(arrowPolygon);
            g2d.drawLine(startLocation.x, startLocation.y, firstX, firstY);
        }
    }
}
