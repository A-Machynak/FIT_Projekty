/**
 * @file MovableCanvas.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Canvas with movable entities and camera
 *
 */

package ija.projekt.uml.view.movable;

import ija.projekt.uml.view.movable.line.MovableLine;
import ija.projekt.uml.utils.Directions;
import ija.projekt.uml.utils.Pair;
import ija.projekt.uml.utils.Utilities;

import javax.swing.*;
import javax.swing.event.EventListenerList;
import javax.swing.event.MouseInputListener;
import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Optional;

/**
 * Canvas used for displaying elements extending MovableEntity
 */
public class MovableCanvas extends JPanel implements MouseInputListener, MouseWheelListener {

    public enum MovableCanvasCommands {
        ENTITY_SELECTED("ENTITY_SELECTED"),
        CANVAS_CLICKED("CANVAS_CLICKED");

        private final String command;
        MovableCanvasCommands(String command) {
            this.command = command;
        }

        public String getCommand() {
            return command;
        }
    }

    protected final Point cameraPosition = new Point(0, 0);

    protected final ArrayList<MovableEntity> entities = new ArrayList<>();
    protected final ArrayList<MovableConnection> connections = new ArrayList<>();
    protected MovableEntity selectedEntity = null;

    protected Point lastLeftClickPosition = new Point(0, 0);

    protected Point startingPosition = new Point(0, 0);
    protected Point endingPosition = new Point(0, 0);

    protected Point childStartingPosition = new Point(0, 0);
    protected Point childEndingPosition = new Point(0, 0);

    protected float currentScalingFactor = 1.0f;
    protected static final float SCALE_FACTOR = 0.01f;

    protected boolean isMiddleDown = false;
    protected boolean isRightDown = false;

    protected final EventListenerList listenerList = new EventListenerList();

    public MovableCanvas() {
        setLayout(null);
        setBackground(Color.lightGray);
        setForeground(Color.lightGray);

        addMouseListener(this);
        addMouseMotionListener(this);
        addMouseWheelListener(this);
    }

    /**
     * Create a connection between entities
     * @param connection connection between entities
     */
    public void addConnection(MovableConnection connection) {
        add(connection.getArrow());
        connections.add(connection);
        updateEntities();
    }

    /**
     * Remove a connection between entities
     * @param connection connection between entities
     */
    public void removeConnection(MovableConnection connection) {
        MovableConnection found = null;
        for(MovableConnection c : connections) {
            if(c.equals(connection)) {
                found = c;
            }
        }
        if(found != null) {
            remove(found.getArrow());
            connections.remove(found);
        }
    }

    /**
     * Get a list of entities
     * @return list of entities
     */
    public java.util.List<MovableEntity> getEntities() {
        return Collections.unmodifiableList(entities);
    }

    /**
     * Get currently selected (last one clicked) entity on canvas
     * @return selected entity
     */
    public MovableEntity getSelectedEntity() {
        return selectedEntity;
    }

    /**
     * Get an entity at certain position on canvas
     * @param position coordinates
     * @return entity or null if not found
     */
    public MovableEntity getEntityAt(Point position) {
        for(MovableEntity c : entities) {
            Pair<Point, Point> rect = c.getContainerLocBoundingRectangle();
            Directions dir = Utilities.getPointInRectangle(position, rect.getFirst(), rect.getSecond());
            if(dir != Directions.NONE) {
                return c;
            }
        }

        return null;
    }

    /**
     * Remove selected (last one clicked) entity
     */
    public void removeSelectedEntity() {
        if(selectedEntity != null) {
            remove(selectedEntity);
            removeAllConnectionsWith(selectedEntity);
            entities.remove(selectedEntity);
            selectedEntity = null;
        }
    }

    public void removeEntity(MovableEntity entity) {
        remove(entity);
    }

    /**
     * Remove entity at certain position
     * @param position coordinates
     */
    public void removeEntityAt(Point position) {
        Optional<MovableEntity> child = entities.stream().filter(e -> e.getPosition().equals(position)).findFirst();
        if(child.isPresent()) {
            MovableEntity ch = child.get();
            removeAllConnectionsWith(ch);
            remove(ch);
            entities.remove(ch);
        }
    }

    /**
     * Remove all connections related to certain entity
     * @param entity to remove connections with
     */
    public void removeAllConnectionsWith(MovableEntity entity) {
        ArrayList<MovableConnection> connected = new ArrayList<>();
        for(MovableConnection c : connections) {
            if(c.getFirst().equals(entity) || c.getSecond().equals(entity)) {
                connected.add(c);
                remove(c.getArrow());
            }
        }
        connections.removeAll(connected);
    }

    /**
     * Add entity to canvas to last clicked position
     * @param entity to add
     */
    public void addEntity(MovableEntity entity) {
        // Set it's position to last clicked position
        int revX = cameraPosition.x + lastLeftClickPosition.x;
        int revY = cameraPosition.y + lastLeftClickPosition.y;
        addEntity(entity, new Point(revX, revY));
    }

    /**
     * Add entity to canvas to a specific position
     * @param entity to add
     * @param pos position
     */
    public void addEntity(MovableEntity entity, Point pos) {
        entities.add(entity);
        entity.setPosition(pos);
        add(entity);
        setComponentZOrder(entity, 0);

        entity.addMouseMotionListener(new MouseMotionAdapter() {
            @Override public void mouseDragged(MouseEvent e) { super.mouseDragged(e); childMouseDragged(e); }
        });

        entity.addMouseListener(new MouseAdapter() {
            @Override public void mouseClicked(MouseEvent e) { super.mouseClicked(e); childMouseClicked(e); }
            @Override public void mousePressed(MouseEvent e) { super.mousePressed(e); childMousePressed(e); }
            @Override public void mouseReleased(MouseEvent e) { super.mouseReleased(e); childMouseReleased(e); }
        });

        updateEntities();
    }

    /**
     * Update all entities' positions
     */
    public void updateEntities() {
        cameraPosition.x += startingPosition.x - endingPosition.x;
        cameraPosition.y += startingPosition.y - endingPosition.y;

        startingPosition = endingPosition;

        for(MovableEntity c : entities) {
            if(!c.isMovable()) {
                continue;
            }

            Point cPos = c.getPosition();
            Dimension size = c.getPreferredSize();

            c.scale(currentScalingFactor);

            c.setSize(size.width, size.height);

            int newX = cPos.x - cameraPosition.x - size.width / 2;
            int newY = cPos.y - cameraPosition.y - size.height / 2;

            c.setLocation(newX, newY);
        }

        for(MovableConnection c : connections) {
            MovableLine a = c.getArrow();
            Point aPos = a.getPosition();

            int newX = aPos.x - cameraPosition.x;
            int newY = aPos.y - cameraPosition.y;

            a.setLocation(newX, newY);
            a.updateArrow();
        }
    }

    /**
     * Move entity by delta calculated during drag event
     * @param entity to move
     */
    private void moveEntity(MovableEntity entity) {
        Point cPos = entity.getPosition();
        int deltax = childEndingPosition.x - childStartingPosition.x;
        int deltay = childEndingPosition.y - childStartingPosition.y;

        cPos.x += deltax;
        cPos.y += deltay;
        entity.setPosition(cPos);

        for(MovableConnection c : connections) {
            if(c.getFirst().equals(entity) || c.getSecond().equals(entity)) {
                c.recalculateArrow();
            }
        }

        childStartingPosition = childEndingPosition;
    }

    @Override
    public void mouseClicked(MouseEvent e) {
        callListeners(new ActionEvent(
                this,
                ActionEvent.ACTION_PERFORMED,
                MovableCanvasCommands.CANVAS_CLICKED.getCommand()
        ));
    }

    @Override
    public void mousePressed(MouseEvent e) {
        if(e.getButton() == MouseEvent.BUTTON1) {
            lastLeftClickPosition = e.getPoint();
        } else if(e.getButton() == MouseEvent.BUTTON2) {
            isMiddleDown = true;

            startingPosition = e.getPoint();
            endingPosition = startingPosition;
        } else if(e.getButton() == MouseEvent.BUTTON3) {
            isRightDown = true;

            startingPosition = e.getPoint();
            endingPosition = startingPosition;
        }
    }

    @Override
    public void mouseReleased(MouseEvent e) {
        if(e.getButton() == MouseEvent.BUTTON2) {
            isMiddleDown = false;
            endingPosition = e.getPoint();
        } else if(e.getButton() == MouseEvent.BUTTON3) {
            isRightDown = false;
            endingPosition = e.getPoint();
        }
    }

    @Override
    public void mouseEntered(MouseEvent e) {
        // Intentionally empty
    }

    @Override
    public void mouseExited(MouseEvent e) {
        // Intentionally empty
    }

    @Override
    public void mouseDragged(MouseEvent e) {
        // e.getButton() doesn't work - shows button "0" all the time
        //System.out.println(e.getButton());
        if(isMiddleDown) {
            endingPosition = e.getPoint();
            updateEntities();
            System.out.println("MID");
        } else if(isRightDown) {

        }
    }

    @Override
    public void mouseMoved(MouseEvent e) {
        // Intentionally empty
    }

    @Override
    public void mouseWheelMoved(MouseWheelEvent e) {
        currentScalingFactor = currentScalingFactor - (e.getUnitsToScroll() * SCALE_FACTOR);
        if(currentScalingFactor < 0.2f) {
            currentScalingFactor = 0.2f;
        } else {
            updateEntities();
        }
    }

    private void childMouseClicked(MouseEvent e) {
        if(e.getSource() instanceof MovableEntity) {
            selectedEntity = (MovableEntity) e.getSource();
            callListeners(new ActionEvent(
                    selectedEntity,
                    ActionEvent.ACTION_PERFORMED,
                    MovableCanvasCommands.ENTITY_SELECTED.getCommand()));
        }
    }

    private void childMousePressed(MouseEvent e) {
        if (e.getSource() instanceof MovableEntity) {
            selectedEntity = (MovableEntity) e.getSource();
        } else {
            selectedEntity = null;
        }
        childStartingPosition = e.getLocationOnScreen();
        childEndingPosition = childStartingPosition;
    }

    private void childMouseReleased(MouseEvent e) {
        childEndingPosition = e.getLocationOnScreen();
    }

    private void childMouseDragged(MouseEvent e) {
        if(e.getSource() instanceof Container) {
            childEndingPosition = e.getLocationOnScreen();
            if(selectedEntity != null) {
                moveEntity(selectedEntity);
                updateEntities();
            }
        }
    }

    /**
     * Add a listener
     * @param listener listener to add
     */
    public void addListener(ActionListener listener) {
        listenerList.add(ActionListener.class, listener);
    }

    private void callListeners(ActionEvent e) {
        for(ActionListener l : listenerList.getListeners(ActionListener.class)) {
            l.actionPerformed(e);
        }
    }
}
