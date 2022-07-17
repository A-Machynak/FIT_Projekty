/**
 * @file ClassDiagramController.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief UMLClassDiagram's UI and model controller
 *
 */

package ija.projekt.uml.controller;

import ija.projekt.uml.model.UMLClass;
import ija.projekt.uml.model.diagram.UMLClassDiagram;
import ija.projekt.uml.model.enums.UMLRelationship;
import ija.projekt.uml.utils.Directions;
import ija.projekt.uml.utils.Pair;
import ija.projekt.uml.utils.Utilities;
import ija.projekt.uml.view.content.CanvasClassDiagramView;
import ija.projekt.uml.view.content.ContentSwitchButton;
import ija.projekt.uml.view.movable.*;

import javax.swing.event.EventListenerList;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Optional;

public class ClassDiagramController {
    private final ArrayList<ClassController> classControllers = new ArrayList<>();

    private final CanvasClassDiagramView classDiagramView;
    private final UMLClassDiagram umlClassDiagram;

    private final ContentSwitchButton contentSwitchButton;

    private final EventListenerList listenerList = new EventListenerList();

    public ClassDiagramController(CanvasClassDiagramView classDiagramView) {
        this(classDiagramView, new UMLClassDiagram("Class diagram"));
    }

    public ClassDiagramController(CanvasClassDiagramView classDiagramView, UMLClassDiagram umlClassDiagram) {
        this.classDiagramView = classDiagramView;
        this.umlClassDiagram = umlClassDiagram;
        this.contentSwitchButton = new ContentSwitchButton(classDiagramView, false);

        this.classDiagramView.addListener(this::viewEvent);
    }

    private void viewEvent(ActionEvent e) {
        // delegate
        callListeners(e);
    }

    public CanvasClassDiagramView getClassDiagramView() {
        return classDiagramView;
    }

    public UMLClassDiagram getUmlClassDiagram() {
        return umlClassDiagram;
    }

    public ContentSwitchButton getContentSwitchButton() {
        return contentSwitchButton;
    }

    /**
     * Add new class to the diagram
     * @param umlClass class to add
     * @return false if class with this name already exists, otherwise true
     */
    public boolean addClass(UMLClass umlClass) {
        UMLClass addedClass = umlClassDiagram.addClass(umlClass);
        if(addedClass == null) {
            return false;
        }

        MovableRectangle cv = new MovableRectangle();
        ClassController controller = new ClassController(umlClass, cv);
        classControllers.add(controller);

        classDiagramView.addClass(cv);

        controller.update();
        classDiagramView.getCanvas().updateEntities();
        return true;
    }

    public UMLClass getClassWithName(String name) {
        for(UMLClass c : umlClassDiagram.getClasses()) {
            if(c.getName().equals(name)) {
                return c;
            }
        }
        return null;
    }

    /**
     * Remove class from the diagram
     * @param umlClass class to remove
     */
    public void removeClass(UMLClass umlClass) {
        if(umlClass == null) {
            return;
        }

        Optional<ClassController> optController = classControllers.stream()
                .filter(e -> e.getUmlClass().getName().equals(umlClass.getName()))
                .findFirst();
        if(!optController.isPresent()) {
            return;
        }

        ClassController controller = optController.get();

        MovableRectangle cv = controller.getMovableUMLClass();
        umlClassDiagram.removeClass(umlClass);
        classDiagramView.getCanvas().removeEntityAt(cv.getPosition());

        classControllers.remove(controller);
        classDiagramView.getCanvas().updateEntities();
    }

    /**
     * Update selected UMLClass UI (called after edit)
     */
    public void updateSelectedClass() {
        MovableEntity entity = classDiagramView.getCanvas().getSelectedEntity();
        if(entity == null) {
            return;
        }

        ClassController cntrl = null;
        for(ClassController controller : classControllers) {
            if(controller.getMovableUMLClass() == entity) {
                cntrl = controller;
            }
        }
        if(cntrl != null) {
            cntrl.update();
        }
        classDiagramView.getCanvas().updateEntities();
    }

    /**
     * Remove selected class from the diagram (Called after delete)
     */
    public void removeSelectedClass() {
        MovableEntity entity = classDiagramView.getCanvas().getSelectedEntity();
        if(entity == null) {
            return;
        }

        Point pos = entity.getPosition();

        ClassController found = null;
        for(ClassController controller : classControllers) {
            if(controller.getMovableUMLClass().getPosition().equals(pos)) {
                found = controller;
            }
        }

        if(found != null) {
            umlClassDiagram.removeClass(found.getUmlClass());
            classControllers.remove(found);
            classDiagramView.getCanvas().removeSelectedEntity();
        }
    }

    /**
     * Get UMLClass's UI representation
     * @param c UMLClass to look for
     * @return MovableEntity or null if not found
     */
    public MovableEntity getMovableWithClass(UMLClass c) {
        for(ClassController controller : classControllers) {
            if(controller.getUmlClass().equals(c)) {
                return controller.getMovableUMLClass();
            }
        }
        return null;
    }

    /**
     * Get UMLClass at certain position in UI diagram
     * @param point Position
     * @return UMLClass or null if not found
     */
    public UMLClass getClassAt(Point point) {
        for(ClassController controller : classControllers) {
            Pair<Point, Point> rect = controller.getMovableUMLClass().getBoundingPoints();
            if(Utilities.getPointInRectangle(point, rect.getFirst(), rect.getSecond()) != Directions.NONE) {
                return controller.getUmlClass();
            }
        }
        return null;
    }

    /**
     * Create relationship between UMLClasses
     * @param c1 First class
     * @param c2 Second class
     * @param relationship UMLRelationship
     */
    public void addConnection(UMLClass c1, UMLClass c2, UMLRelationship relationship) {
        // update model
        umlClassDiagram.addConnection(c1, c2, relationship);

        // update ui
        MovableConnectionLines arrow;
        if(relationship == UMLRelationship.AGGREGATION) {
            arrow = MovableConnectionLines.DIAMOND;
        } else if(relationship == UMLRelationship.COMPOSITION) {
            arrow = MovableConnectionLines.FILLED_DIAMOND;
        } else if(relationship == UMLRelationship.GENERALIZATION) {
            arrow = MovableConnectionLines.TRIANGLE;
        } else {
            arrow = MovableConnectionLines.STRAIGHT;
        }

        MovableConnection connection = new MovableConnection(getMovableWithClass(c1), getMovableWithClass(c2), arrow);

        classDiagramView.getCanvas().addConnection(connection);
    }

    /**
     * Create relationship between UMLClasses
     * @param c1 First entity
     * @param c2 Second entity
     * @param relationship UMLRelationship
     */
    public void addConnection(MovableEntity c1, MovableEntity c2, UMLRelationship relationship) {
        // update model
        umlClassDiagram.addConnection(getClassAt(c1.getPosition()), getClassAt(c2.getPosition()), relationship);

        // update ui
        MovableConnectionLines arrow;
        if(relationship == UMLRelationship.AGGREGATION) {
            arrow = MovableConnectionLines.DIAMOND;
        } else if(relationship == UMLRelationship.COMPOSITION) {
            arrow = MovableConnectionLines.FILLED_DIAMOND;
        } else if(relationship == UMLRelationship.GENERALIZATION) {
            arrow = MovableConnectionLines.TRIANGLE;
        } else {
            arrow = MovableConnectionLines.STRAIGHT;
        }

        MovableConnection connection = new MovableConnection(c1, c2, arrow);
        classDiagramView.getCanvas().addConnection(connection);
    }

    public void addListener(ActionListener l) {
        listenerList.add(ActionListener.class, l);
    }

    private void callListeners(ActionEvent e) {
        for(ActionListener l : listenerList.getListeners(ActionListener.class)) {
            l.actionPerformed(e);
        }
    }
}
