/**
 * @file CanvasView.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Basic canvas view
 *
 */

package ija.projekt.uml.view.content;

import ija.projekt.uml.view.MainWindow;
import ija.projekt.uml.view.movable.MovableCanvas;

import javax.swing.*;
import javax.swing.event.EventListenerList;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class CanvasView extends JPanel {
    protected MainWindow window;

    protected MovableCanvas canvas;
    protected Container leftSidebar;
    protected Container rightSidebar;

    protected EventListenerList listenerList = new EventListenerList();

    public CanvasView(MainWindow window) {
        this.window = window;
        this.canvas = new MovableCanvas();
        this.leftSidebar = new JPanel(new BorderLayout());
        this.rightSidebar = new JPanel(new BorderLayout());

        setLayout(new BorderLayout());

        add(canvas, BorderLayout.CENTER);
        add(leftSidebar, BorderLayout.LINE_START);
        add(rightSidebar, BorderLayout.LINE_END);
    }

    public MovableCanvas getCanvas() {
        return canvas;
    }

    public void setRightSidebar(Container rightSidebar) {
        this.rightSidebar.removeAll();
        this.rightSidebar.add(rightSidebar);
    }

    public void setLeftSidebar(Container leftSidebar) {
        this.leftSidebar.removeAll();
        this.leftSidebar.add(leftSidebar);
    }

    public Container getRightSidebar() {
        return rightSidebar;
    }

    public Container getLeftSidebar() {
        return leftSidebar;
    }

    public void showCanvas() {
        window.setCenterContainer(this);
    }

    public void addListener(ActionListener listener) {
        listenerList.add(ActionListener.class, listener);
    }

    protected void callListeners(ActionEvent e) {
        for(ActionListener l : listenerList.getListeners(ActionListener.class)) {
            l.actionPerformed(e);
        }
    }
}
