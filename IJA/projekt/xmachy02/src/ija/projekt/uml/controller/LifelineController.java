package ija.projekt.uml.controller;

import ija.projekt.uml.model.UMLClass;
import ija.projekt.uml.model.UMLLifeline;
import ija.projekt.uml.utils.Pair;
import ija.projekt.uml.view.movable.MovableFocusOfControl;
import ija.projekt.uml.view.movable.MovableLifeline;

import javax.swing.event.EventListenerList;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

public class LifelineController {
    private ArrayList<Pair<UMLLifeline.UMLFocusOfControl, MovableFocusOfControl>> focsList = new ArrayList<>();
    private final UMLLifeline umlLifeline;
    private final MovableLifeline lifeline;
    private String instanceName = "";

    private final EventListenerList listenerList = new EventListenerList();

    public LifelineController(UMLClass umlClass) {
        umlLifeline = new UMLLifeline(umlClass);
        lifeline = new MovableLifeline(new Point(0, 0), ":" + umlClass.getName());
    }

    public MovableFocusOfControl addFOC() {
        var foc = new UMLLifeline.UMLFocusOfControl();
        var mfoc = new MovableFocusOfControl();
        lifeline.bindFOC(mfoc);

        focsList.add(new Pair<>(foc, mfoc));

        return mfoc;
    }

    public UMLLifeline getUmlLifeline() {
        return umlLifeline;
    }

    public MovableLifeline getLifeline() {
        return lifeline;
    }

    public void setInstanceName(String instanceName) {
        this.instanceName = instanceName;
    }

    public String getInstanceName() {
        return instanceName;
    }

    public void updateUI() {
        var umlClass = umlLifeline.getUmlClass();
        if(umlClass == null) {
            return;
        }
        lifeline.setName(instanceName + ":" + umlClass.getName());
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
