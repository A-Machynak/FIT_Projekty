/**
 * @file SequenceDiagramController.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Sequential diagram's controller (Unfinished)
 *
 */

package ija.projekt.uml.controller;

import ija.projekt.uml.model.*;
import ija.projekt.uml.model.diagram.UMLSeqDiagram;
import ija.projekt.uml.model.enums.UMLAccessModifier;
import ija.projekt.uml.utils.Pair;
import ija.projekt.uml.view.content.CanvasSequenceDiagramView;
import ija.projekt.uml.view.content.ContentSwitchButton;
import ija.projekt.uml.view.movable.*;
import ija.projekt.uml.view.movable.line.MovableLineWithMessage;

import javax.swing.*;
import javax.swing.event.EventListenerList;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

public class SequenceDiagramController {
    private final ClassDiagramController classDiagramController;

    private final UMLSeqDiagram umlSeqDiagram;
    private final CanvasSequenceDiagramView sequenceDiagramView;
    private final ContentSwitchButton contentSwitchButton;

    private final ArrayList<LifelineController> lifelineControllers = new ArrayList<>();
    private final ArrayList<MessageController> messageControllers = new ArrayList<>();
    //private final ArrayList<Pair<UMLOperation, MovableLineWithMessage>> operations = new ArrayList<>();
    private MovableEntity selected = null;

    private final EventListenerList listenerList = new EventListenerList();

    public SequenceDiagramController(ClassDiagramController classDiagramController, CanvasSequenceDiagramView sequenceDiagramView) {
        this.classDiagramController = classDiagramController;
        this.sequenceDiagramView = sequenceDiagramView;
        this.contentSwitchButton = new ContentSwitchButton(this.sequenceDiagramView, true);
        this.umlSeqDiagram = new UMLSeqDiagram("Sequence diagram");

        this.contentSwitchButton.addActionListener(this::contentSwitchAction);
        this.sequenceDiagramView.addListener(this::sequenceDiagramViewAction);
    }

    public CanvasSequenceDiagramView getSequenceDiagramView() {
        return sequenceDiagramView;
    }

    public ContentSwitchButton getContentSwitchButton() {
        return contentSwitchButton;
    }

    public ClassDiagramController getClassDiagramController() {
        return classDiagramController;
    }

    /**
     * Remove all lifelines related to a certain uml class
     * @param c umlclass
     */
    public void removeClass(UMLClass c) {
        var toRemove = new ArrayList<LifelineController>();

        for(var lc : lifelineControllers) {
            if(!lc.getUmlLifeline().getUmlClass().equals(c)) {
                continue;
            }

            var lifeline = lc.getLifeline();

            // remove all related focs
            for(var foc : lifeline.getFOCs()) {
                sequenceDiagramView.getCanvas().removeEntity(foc);
            }
            // remove all related messages
            for(var mc : messageControllers) {
                if(mc.getUmlMessage().getSender().getUmlClass().equals(c)
                || mc.getUmlMessage().getReceiver().getUmlClass().equals(c)) {
                    sequenceDiagramView.getCanvas().removeEntity(mc.getLine());
                }
            }
            sequenceDiagramView.getCanvas().removeEntity(lc.getLifeline());
            toRemove.add(lc);
        }

        for(var lc : toRemove) {
            lifelineControllers.remove(lc);
        }
    }

    public void removeLifeline(UMLLifeline l) {
        for(var lc : lifelineControllers) {
            if(lc.getUmlLifeline().equals(l)) {
                var lifeline = lc.getLifeline();

                // remove all related focs
                for(var foc : lifeline.getFOCs()) {
                    sequenceDiagramView.getCanvas().removeEntity(foc);
                }

                // remove all related messages
                for(var mc : messageControllers) {
                    if(mc.getUmlMessage().getSender().getUmlClass().equals(l.getUmlClass())
                            || mc.getUmlMessage().getReceiver().getUmlClass().equals(l.getUmlClass())) {
                        sequenceDiagramView.getCanvas().removeEntity(mc.getLine());
                    }
                }
                sequenceDiagramView.getCanvas().removeEntity(lc.getLifeline());
                lifelineControllers.remove(lc);
                break;
            }
        }
    }

    public void updateUI() {
        for(var lc : lifelineControllers) {
            lc.updateUI();
        }
        for(var msgc : messageControllers) {
            msgc.updateMessage();
        }
    }

    private void contentSwitchAction(ActionEvent e) {
        if(!e.getActionCommand().equals("Switch")) {
            return;
        }

        // Get all class names
        var classDiagram = classDiagramController.getUmlClassDiagram();
        var classList = classDiagram.getClasses();
        var stringList = new ArrayList<String>();
        for(UMLClass c : classList) {
            stringList.add(c.getName());
        }

        sequenceDiagramView.setCustomClassNames(stringList);
        sequenceDiagramView.updateLeftSidebarButtons();
    }

    private void sequenceDiagramViewAction(ActionEvent e) {
        String cmd = e.getActionCommand();
        if(cmd.startsWith("Class ")) {
            // Create lifeline for a class
            createLifeline(cmd.substring(6), "");
        } else if(cmd.equals("none")) {

        } else if(cmd.equals("focus of control")) {
            // Create focus of control for a lifeline
            if(e.getSource() instanceof MovableLifeline) {
                createFOC((MovableLifeline)e.getSource());
            }
        } else if(cmd.equals("synchronous message")) {
            if(!(e.getSource() instanceof MovableLifeline || e.getSource() instanceof MovableFocusOfControl)) {
                return;
            }

            if(selected == null || selected == e.getSource()) {
                selected = (MovableEntity) e.getSource();
            } else {
                syncMessage(selected, (MovableEntity) e.getSource());
                selected = null;
            }
        } else if(cmd.equals("asynchronous message")) {
            if(!(e.getSource() instanceof MovableLifeline || e.getSource() instanceof MovableFocusOfControl)) {
                return;
            }

            if(selected == null || selected == e.getSource()) {
                selected = (MovableEntity) e.getSource();
            } else {
                asyncMessage(selected, (MovableEntity) e.getSource());
                selected = null;
            }
        } else if(cmd.equals("return message")) {
            if(!(e.getSource() instanceof MovableLifeline || e.getSource() instanceof MovableFocusOfControl)) {
                return;
            }

            if(selected == null || selected == e.getSource()) {
                selected = (MovableEntity) e.getSource();
            } else {
                returnMessage(selected, (MovableEntity) e.getSource());
                selected = null;
            }
        } else if(cmd.equals("unselect")) {
            selected = null;
        }

        callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "revalidate"));
    }

    private void createLifeline(String name, String instanceName) {
        var umlClass = classDiagramController.getClassWithName(name);
        if(umlClass == null) {
            // unexpected
            return;
        }
        var lc = new LifelineController(umlClass);

        // update model
        umlSeqDiagram.addLifeline(lc.getUmlLifeline());

        // update UI
        sequenceDiagramView.addLifeline(lc.getLifeline());
        sequenceDiagramView.getCanvas().updateEntities();
        lc.getLifeline().addListener(this::lifelineAction);

        lifelineControllers.add(lc);
    }

    private void lifelineAction(ActionEvent e) {
        if(!(e.getSource() instanceof MovableLifeline)) {
            System.out.println(e.getSource());
            return;
        }
        var ml = (MovableLifeline) e.getSource();
        var mlc = getLifelineController(ml);
        if(mlc == null) {
            return;
        }

        removeLifeline(mlc.getUmlLifeline());
    }

    private void createFOC(MovableLifeline ml) {
        var lc = getLifelineController(ml);
        if(lc == null) {
            System.out.println("NULL");
            return;
        }

        // update model and UI
        var mfoc = lc.addFOC();
        sequenceDiagramView.getCanvas().addEntity(mfoc);
    }

    /**
     * Get lifeline's model representation
     * @param ml lifeline
     * @return lifeline model
     */
    private UMLLifeline getLifelineModel(MovableLifeline ml) {
        var lc = getLifelineController(ml);
        return (lc == null) ? null : lc.getUmlLifeline();
    }

    /**
     * Get lifeline's GUI representation
     * @param l lifeline model
     * @return lifeline
     */
    private MovableLifeline getLifelineUI(UMLLifeline l) {
        var lc = getLifelineController(l);
        return (lc == null) ? null : lc.getLifeline();
    }

    /**
     * Get controller which belongs to a certain lifeline
     * @param l lifeline
     * @return controller or null if not found
     */
    private LifelineController getLifelineController(UMLLifeline l) {
        for(var lc : lifelineControllers) {
            if(lc.getUmlLifeline().equals(l)) {
                return lc;
            }
        }
        return null;
    }

    /**
     * Get controller which belongs to a certain lifeline or foc
     * @param ml lifeline
     * @return controller or null if not found
     */
    private LifelineController getLifelineController(MovableEntity ml) {
        for(var lc : lifelineControllers) {
            if(lc.getLifeline().equals(ml)) {
                return lc;
            }
            for(var foc : lc.getLifeline().getFOCs()) {
                if(foc.equals(ml)) {
                    return lc;
                }
            }
        }
        return null;
    }

    private void syncMessage(MovableEntity e1, MovableEntity e2) {
        createMessage(e1, e2, MovableLineWithMessage.LineType.NORMAL_TRIANGLE);
    }

    private void asyncMessage(MovableEntity e1, MovableEntity e2) {
        createMessage(e1, e2, MovableLineWithMessage.LineType.DASHED_NORMAL);
    }

    private void returnMessage(MovableEntity e1, MovableEntity e2) {
        createMessage(e1, e2, MovableLineWithMessage.LineType.NORMAL);
    }

    private void createMessage(MovableEntity e1, MovableEntity e2, MovableLineWithMessage.LineType type) {
        var lc = getLifelineController(e1);
        var lc2 = getLifelineController(e2);
        if(lc == null || lc2 == null) {
            return;
        }

        // Get all the available methods from this class
        var methods = lc2.getUmlLifeline().getUmlClass().getMethods();
        var possibilities = new ArrayList<UMLOperation>();

        var nullOp = new UMLOperation("", new UMLClassifier(""), UMLAccessModifier.PUBLIC);
        possibilities.add(nullOp);
        var createOp = new UMLOperation("<<create>>", new UMLClassifier(""), UMLAccessModifier.PUBLIC);
        possibilities.add(createOp);
        var deleteOp = new UMLOperation("<<delete>>", new UMLClassifier(""), UMLAccessModifier.PUBLIC);
        possibilities.add(deleteOp);

        for(var m : methods) {
            if(!m.getAccessModifier().equals(UMLAccessModifier.PRIVATE)) {
                possibilities.add(m);
            }
        }

        // Prompt user to select a method
        UMLOperation op = (UMLOperation) JOptionPane.showInputDialog(sequenceDiagramView,
                "Select method",
                "Method selection",
                JOptionPane.PLAIN_MESSAGE,
                null,
                possibilities.toArray(),
                nullOp);
        if(op == null) {
            return;
        }

        var mlwm = new MovableLineWithMessage(e1, e2, type);
        mlwm.setMessage(op.getName());

        var msgc = new MessageController(lc.getUmlLifeline(), lc2.getUmlLifeline(), op, mlwm);
        messageControllers.add(msgc);

        sequenceDiagramView.getCanvas().addEntity(mlwm);
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
