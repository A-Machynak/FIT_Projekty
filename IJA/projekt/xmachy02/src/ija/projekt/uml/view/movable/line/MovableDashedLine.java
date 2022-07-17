package ija.projekt.uml.view.movable.line;

import ija.projekt.uml.view.movable.MovableConnectionLines;

import java.awt.*;

public class MovableDashedLine extends MovableLine {

    public MovableDashedLine(Point startPos, Point endPos) {
        super(startPos, endPos);
        this.arrowType = MovableConnectionLines.DASHED;
        lineStroke = new BasicStroke(1.f, BasicStroke.CAP_BUTT, BasicStroke.JOIN_MITER, 10.0f, new float[]{10.0f}, 0.0f);
        updateArrow();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        if(arrowType == MovableConnectionLines.DASHED) {
            Graphics2D g2d = (Graphics2D) g;

            g2d.setStroke(lineStroke);
            g2d.setColor(Color.black);
            g2d.drawLine(startPositionLocal.x, startPositionLocal.y, endPositionLocal.x, endPositionLocal.y);
        }
    }
}
