/**
 * @file MainController.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Main controller, which takes care of all the other controllers
 *
 */

package ija.projekt.uml.controller;

import ija.projekt.uml.model.UMLClass;
import ija.projekt.uml.model.enums.UMLRelationship;
import ija.projekt.uml.view.MainWindow;
import ija.projekt.uml.view.content.CanvasClassDiagramView;
import ija.projekt.uml.view.content.CanvasSequenceDiagramView;
import ija.projekt.uml.view.content.ContentSwitchButton;
import ija.projekt.uml.view.movable.MovableEntity;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.util.ArrayList;

public class MainController {
    private MainWindow window;
    private ClassDiagramController classDiagramController;
    private SidebarEditorController editorController;

    private final ArrayList<SequenceDiagramController> sequenceDiagramControllers = new ArrayList<>();

    private MovableEntity connectWith = null;

    public MainController(MainWindow window) {
        this.window = window;
        this.editorController = new SidebarEditorController();

        CanvasClassDiagramView view = new CanvasClassDiagramView(this.window);
        view.showCanvas();
        this.classDiagramController = new ClassDiagramController(view);

        this.editorController.addListener(this::editorControllerEvent);
        this.classDiagramController.addListener(this::classDiagramAndViewEvent);

        // Top menu button for adding new sequence diagrams
        JButton addTabButton = new JButton("+");
        addTabButton.setToolTipText("Add sequence diagram");
        addTabButton.addActionListener(e -> addDiagramButtonAction());
        window.addToTopContainer(addTabButton);

        // Create default class diagram tab
        ContentSwitchButton csb = classDiagramController.getContentSwitchButton();
        csb.setText("Class");
        csb.switchContent();
        window.addToTopContainer(csb);
        csb.addActionListener(this::tabButtonAction);
    }

    /**
     * Add new UMLClass to class diagram
     * @param c class to add
     */
    public void addUMLClass(UMLClass c) {
        if(c.getName().equals("")) {
            window.errorDialog("Class name cannot be empty!");
            return;
        }

        boolean added = classDiagramController.addClass(c);

        if(!added) {
            window.errorDialog("Class with this name already exists!");
        }
    }

    /**
     * Called for events coming from editor controller
     * @param e - event
     */
    private void editorControllerEvent(ActionEvent e) {
        String cmd = e.getActionCommand();
        if(cmd.equals("Revalidate")) {
            window.revalidate();
            return;
        }

        if(cmd.equals("Create")) {
            editorController.updateClass();
            addUMLClass(editorController.getUmlClass());
        } else if(cmd.equals("Apply")) {
            if(!editorController.isClassNameEqual()) {
                UMLClass c = classDiagramController.getClassWithName(editorController.getEditor().getName());
                if(c != null) {
                    window.errorDialog("Class with this name already exists!");
                    return;
                }
            }

            editorController.updateClass();
            classDiagramController.updateSelectedClass();
        } else if(cmd.equals("Cancel")) {
            // empty
        } else if(cmd.equals("Delete")) {
            var umlClass = editorController.getUmlClass();
            for(var sdc : sequenceDiagramControllers) {
                sdc.removeClass(umlClass);
            }
            classDiagramController.removeClass(umlClass);
        }
        classDiagramController.getClassDiagramView().getRightSidebar().removeAll();
        window.revalidate();
    }

    /**
     * Called for events coming from class diagram and class view
     * @param e - event
     */
    private void classDiagramAndViewEvent(ActionEvent e) {
        String cmd = e.getActionCommand();
        if(cmd.equals("Revalidate")) {
            window.revalidate();
            return;
        }

        // Create class/interface
        if(cmd.equals("Class") || cmd.equals("Interface")) {
            editorController.setUmlClass(new UMLClass(""));
            editorController.updateEditor();
            editorController.setEditing(false);

            classDiagramController.getClassDiagramView().setRightSidebar(editorController.getEditor());
            window.revalidate();
        } else if(e.getSource() instanceof MovableEntity) {
            // Edit or create relationship

            MovableEntity entity = (MovableEntity)e.getSource();
            if(cmd.equals("Edit")) {
                UMLClass c = classDiagramController.getClassAt(entity.getPosition());
                editorController.setUmlClass(c);
                editorController.updateEditor();
                editorController.setEditing(true);

                classDiagramController.getClassDiagramView().setRightSidebar(editorController.getEditor());
                window.revalidate();
            } else if(cmd.equals("Association") || cmd.equals("Aggregation")
                    || cmd.equals("Composition") || cmd.equals("Generalization")) {
                if(connectWith == null) {
                    connectWith = entity;
                } else {
                    classDiagramController.addConnection(connectWith, entity, UMLRelationship.fromString(cmd));
                    connectWith = null;
                }
            }
        }
    }

    /**
     * Add new sequence diagram
     * @return Added sequence diagram
     */
    public SequenceDiagramController addNewSequenceDiagram() {
        CanvasSequenceDiagramView diagramView = new CanvasSequenceDiagramView(window);
        SequenceDiagramController diagramController = new SequenceDiagramController(classDiagramController, diagramView);

        sequenceDiagramControllers.add(diagramController);

        return diagramController;
    }

    /**
     * Button action - add sequence diagram
     */
    private void addDiagramButtonAction() {
        window.setInfoText("New sequence diagram added");

        // Create new sequence diagram
        SequenceDiagramController sdc = addNewSequenceDiagram();

        ContentSwitchButton csb = sdc.getContentSwitchButton();
        csb.setText("Sequence diagram");
        // Add a button to top container to switch to this diagram
        window.addToTopContainer(csb);

        csb.addActionListener(this::tabButtonAction);
        sdc.addListener(this::sequenceDiagramEvent);

        window.revalidate();
    }

    private void sequenceDiagramEvent(ActionEvent e) {
        if(e.getActionCommand().equals("revalidate")) {
            window.revalidate();
        }
    }

    /**
     * ContentSwitchButton action - switching and deleting diagrams
     */
    private void tabButtonAction(ActionEvent e) {
        if(!(e.getSource() instanceof ContentSwitchButton)) {
            return;
        }

        if(e.getActionCommand().equals("Switch")) {
            tabButtonSwitch((ContentSwitchButton) e.getSource());
        } else if(e.getActionCommand().equals("Delete")) {
            tabButtonDelete((ContentSwitchButton) e.getSource());
        }
    }

    /**
     * Helper function for tabButtonAction - switching between diagrams
     * @param csb source button
     */
    private void tabButtonSwitch(ContentSwitchButton csb) {
        window.setCenterContainer(csb.getContent());

        for(SequenceDiagramController sdc : sequenceDiagramControllers) {
            ContentSwitchButton other = sdc.getContentSwitchButton();
            if(csb.equals(other)) {
                sdc.updateUI();
                window.setCenterContainer(sdc.getSequenceDiagramView());
                window.revalidate();
            } else {
                other.unswitch();
            }
        }

        if(!(csb.getContent() instanceof CanvasClassDiagramView)) {
            classDiagramController.getContentSwitchButton().unswitch();
        }
    }

    /**
     * Helper function for tabButtonAction - deleting sequence diagrams
     * @param csb source button
     */
    private void tabButtonDelete(ContentSwitchButton csb) {
        Container c = csb.getContent();
        if(!(c instanceof CanvasSequenceDiagramView)) {
            return;
        }

        for(SequenceDiagramController sdc : sequenceDiagramControllers) {
            if(sdc.getSequenceDiagramView().equals(c)) {
                window.removeFromTopContainer(csb);
                sequenceDiagramControllers.remove(sdc);
                return;
            }
        }
    }
}
