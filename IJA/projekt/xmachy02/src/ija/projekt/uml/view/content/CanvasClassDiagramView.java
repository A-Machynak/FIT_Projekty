/**
 * @file CanvasClassDiagramView.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Class diagram's view
 *
 */

package ija.projekt.uml.view.content;

import ija.projekt.uml.view.MainWindow;
import ija.projekt.uml.view.movable.MovableCanvas;
import ija.projekt.uml.view.movable.MovableEntity;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;

public class CanvasClassDiagramView extends CanvasView {
    /**
     * Buttons and their appropriate commands
     */
    private enum SelectionButtons {
        NONE("None"),
        CLASS("Class"),
        INTERFACE("Interface"),
        ASSOCIATION("Association"),
        AGGREGATION("Aggregation"),
        COMPOSITION("Composition"),
        GENERALIZATION("Generalization");

        private final String description;
        SelectionButtons(String text) {
            this.description = text;
        }

        public String getDescription() { return description; }
    }

    private SelectionButtons selectedButton = SelectionButtons.NONE;

    public CanvasClassDiagramView(MainWindow window) {
        super(window);
        init();
    }

    public void addClass(MovableEntity entity) {
        canvas.addEntity(entity);
    }

    private void init() {
        leftSidebar.setLayout(new BoxLayout(leftSidebar, BoxLayout.Y_AXIS));
        rightSidebar.setPreferredSize(new Dimension(300, rightSidebar.getPreferredSize().height));

        canvas.addListener(this::canvasAction);
        createLeftSidebarButtons();
    }
    
    private void createLeftSidebarButtons() {
        ButtonGroup actionSelector = new ButtonGroup();

        for(SelectionButtons b : SelectionButtons.values()) {
            JRadioButton button = new JRadioButton(b.getDescription());
            button.addActionListener(e -> chooseButtonAction(b));
            leftSidebar.add(button);
            actionSelector.add(button);

            if(actionSelector.getSelection() == null) {
                button.setSelected(true);
            }
        }
    }

    private void chooseButtonAction(SelectionButtons button) {
        window.setInfoText("Selected " + button.getDescription().toLowerCase());
        selectedButton = button;
    }

    private void canvasAction(ActionEvent e) {
        String canvasClickedCommand = MovableCanvas.MovableCanvasCommands.CANVAS_CLICKED.getCommand();
        String entitySelectedCommand = MovableCanvas.MovableCanvasCommands.ENTITY_SELECTED.getCommand();

        if(e.getActionCommand().equals(canvasClickedCommand)) {
            if(selectedButton == SelectionButtons.CLASS) {
                window.setInfoText("Creating class");
            } else if(selectedButton == SelectionButtons.INTERFACE) {
                window.setInfoText("Creating interface");
            } else if(selectedButton == SelectionButtons.ASSOCIATION) {
                window.setInfoText("Creating association relationship");
            } else if(selectedButton == SelectionButtons.AGGREGATION) {
                window.setInfoText("Creating aggregation relationship");
            } else if(selectedButton == SelectionButtons.COMPOSITION) {
                window.setInfoText("Creating composition relationship");
            } else if(selectedButton == SelectionButtons.GENERALIZATION) {
                window.setInfoText("Creating generalization relationship");
            }
            callListeners(new ActionEvent(e.getSource(), ActionEvent.ACTION_PERFORMED, selectedButton.getDescription()));
        } else if(e.getActionCommand().equals(entitySelectedCommand)) {
            if(selectedButton == SelectionButtons.ASSOCIATION) {
                window.setInfoText("Creating association relationship");
            } else if(selectedButton == SelectionButtons.AGGREGATION) {
                window.setInfoText("Creating aggregation relationship");
            } else if(selectedButton == SelectionButtons.COMPOSITION) {
                window.setInfoText("Creating composition relationship");
            } else if(selectedButton == SelectionButtons.GENERALIZATION) {
                window.setInfoText("Creating generalization relationship");
            } else if(selectedButton == SelectionButtons.NONE
                    || selectedButton == SelectionButtons.CLASS
                    || selectedButton == SelectionButtons.INTERFACE) {
                callListeners(new ActionEvent(e.getSource(), ActionEvent.ACTION_PERFORMED, "Edit"));
                return;
            }
            callListeners(new ActionEvent(e.getSource(), ActionEvent.ACTION_PERFORMED, selectedButton.getDescription()));
        }
    }
}
