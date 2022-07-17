/**
 * @file CanvasSequenceDiagramView.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Sequence diagram's view
 *
 */

package ija.projekt.uml.view.content;

import ija.projekt.uml.view.MainWindow;
import ija.projekt.uml.view.movable.MovableCanvas;
import ija.projekt.uml.view.movable.MovableEntity;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.util.ArrayList;

public class CanvasSequenceDiagramView extends CanvasView {
    /**
     * Buttons and their appropriate commands
     */
    private enum SelectionButtons {
        NONE("none"),
        FOC("focus of control"),
        SYN_MSG("synchronous message"),
        ASYN_MSG("asynchronous message"),
        RET_MSG("return message"),
        CLASS("");

        private final String description;
        SelectionButtons(String text) {
            this.description = text;
        }

        public String getDescription() { return description; }
    }

    /**
     * Contains the class names from class diagram. Value is set from the controller
     */
    private ArrayList<String> customClassNames = new ArrayList<>();

    private SelectionButtons selectedButton = SelectionButtons.NONE;
    private String selectedClass = "";

    public CanvasSequenceDiagramView(MainWindow window) {
        super(window);
        init();
    }

    private void init() {
        remove(leftSidebar);

        // change left sidebar to scrollpane
        leftSidebar.setLayout(new BoxLayout(leftSidebar, BoxLayout.Y_AXIS));
        JScrollPane leftScrollPane = new JScrollPane(leftSidebar,
                ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED,
                ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        add(leftScrollPane, BorderLayout.LINE_START);

        rightSidebar.setPreferredSize(new Dimension(300, rightSidebar.getPreferredSize().height));

        canvas.addListener(this::canvasAction);
        updateLeftSidebarButtons();
    }

    public void addLifeline(MovableEntity l) {
        canvas.addEntity(l);
    }

    public void setCustomClassNames(ArrayList<String> customClassNames) {
        this.customClassNames = customClassNames;
    }

    public void updateLeftSidebarButtons() {
        leftSidebar.removeAll();

        ButtonGroup actionSelector = new ButtonGroup();
        // buttons
        for(SelectionButtons b : SelectionButtons.values()) {
            if(b == SelectionButtons.CLASS) continue;

            JRadioButton button = new JRadioButton(b.getDescription());
            button.addActionListener(e -> chooseButtonAction(b, ""));
            leftSidebar.add(button);
            actionSelector.add(button);

            if(actionSelector.getSelection() == null) {
                button.setSelected(true);
            }
        }

        // Insert the class names
        leftSidebar.add(new JSeparator());

        for(String s : customClassNames) {
            JRadioButton button = new JRadioButton(s);
            button.addActionListener(e -> chooseButtonAction(SelectionButtons.CLASS, s));
            leftSidebar.add(button);
            actionSelector.add(button);
        }

        window.revalidate();
    }

    private void chooseButtonAction(SelectionButtons button, String className) {
        if(button == SelectionButtons.CLASS) {
            window.setInfoText("Selected class " + className);
        } else {
            window.setInfoText("Selected " + button.getDescription().toLowerCase());
        }
        selectedButton = button;
        selectedClass = className;

        callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "unselect"));
    }

    private void canvasAction(ActionEvent e) {
        String canvasClickedCommand = MovableCanvas.MovableCanvasCommands.CANVAS_CLICKED.getCommand();
        String entitySelectedCommand = MovableCanvas.MovableCanvasCommands.ENTITY_SELECTED.getCommand();

        if(e.getActionCommand().equals(canvasClickedCommand)) {
            if(selectedButton == SelectionButtons.CLASS) {
                window.setInfoText("Creating lifeline for class " + selectedClass);
                callListeners(new ActionEvent(e.getSource(), ActionEvent.ACTION_PERFORMED, "Class " + selectedClass));
            } else {
                window.setInfoText("Creating " + selectedButton.getDescription());
                callListeners(new ActionEvent(e.getSource(), ActionEvent.ACTION_PERFORMED, selectedButton.getDescription()));
            }
        } else if(e.getActionCommand().equals(entitySelectedCommand)) {
            if(selectedButton != SelectionButtons.CLASS) {
                window.setInfoText("Creating " + selectedButton.getDescription());
                callListeners(new ActionEvent(e.getSource(), ActionEvent.ACTION_PERFORMED, selectedButton.getDescription()));
            }
        }
    }
}
