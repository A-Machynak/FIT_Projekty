/**
 * @file SidebarEditorController.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief SidebarEditor's controller
 *
 */

package ija.projekt.uml.controller;

import ija.projekt.uml.model.UMLAttribute;
import ija.projekt.uml.model.UMLClass;
import ija.projekt.uml.model.UMLClassifier;
import ija.projekt.uml.model.UMLOperation;
import ija.projekt.uml.model.enums.UMLAccessModifier;
import ija.projekt.uml.utils.FunctionParser;
import ija.projekt.uml.utils.Pair;
import ija.projekt.uml.view.content.SidebarClassEditor;

import javax.swing.event.EventListenerList;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

public class SidebarEditorController {

    private final SidebarClassEditor editor;
    private boolean editing = false;
    private UMLClass umlClass;

    private final EventListenerList listenerList = new EventListenerList();

    public SidebarEditorController() {
        this(new SidebarClassEditor());
    }

    public SidebarEditorController(SidebarClassEditor editor) {
        this.editor = editor;
        this.umlClass = new UMLClass("");
        
        editor.addListener(this::editClassAction);
    }

    /**
     * Checks if UI and UMLClass names are equal
     * @return true if UI and UMLClass names are equal, otherwise false
     */
    public boolean isClassNameEqual() {
        return umlClass.getName().equals(editor.getName());
    }

    /**
     * Update UMLClass based on SidebarEditor
     */
    public void updateClass() {
        if(umlClass == null) {
            return;
        }
        umlClass.setName(editor.getName());

        // Remove attributes and add them again based on editor values
        umlClass.removeAttributes();
        for(Pair<String, Pair<String, String>> p : editor.getAttributes()) {
            String name = p.getFirst();
            String type = p.getSecond().getFirst();
            String accessModifier = p.getSecond().getSecond();
            if(name.equals("")) {
                continue;
            }

            umlClass.addAttribute(new UMLAttribute(
                    name,
                    new UMLClassifier(type),
                    UMLAccessModifier.toUMLAccessModifier(accessModifier)));
        }

        // Remove methods and add them again based on editor values
        umlClass.removeMethods();
        for(Pair<String, Pair<String, String>> p : editor.getMethods()) {
            String name = p.getFirst();
            String type = p.getSecond().getFirst();
            String accessModifier = p.getSecond().getSecond();
            if(name.equals("")) {
                continue;
            }
            var fn = FunctionParser.parseFunction(name, accessModifier);

            if(fn == null) {
                continue;
            }

            var op = new UMLOperation(
                    fn.getName(),
                    new UMLClassifier(type),
                    UMLAccessModifier.toUMLAccessModifier(fn.getAccessModifier()));
            for(var arg : fn.getArguments()) {
                op.addArgument(new UMLAttribute(arg.getFirst(), new UMLClassifier(arg.getSecond())));
            }

            umlClass.addMethod(op);
        }
    }

    /**
     * Update SidebarEditor based on UMLClass
     */
    public void updateEditor() {
        if(umlClass == null) {
            return;
        }

        editor.reset();

        editor.setClassNameField(umlClass.getName());
        for(UMLAttribute attr : umlClass.getAttributes()) {
            editor.addAttributeField(new Pair<>(
                    attr.getName(),
                    new Pair<>(
                            attr.getType().getName(),
                            attr.getAccessModifier().getName()
                    )
            ));
        }

        for(UMLOperation method : umlClass.getMethods()) {
            StringBuilder name = new StringBuilder(method.getName() + "(");
            var arguments = method.getArguments();
            for(int i = 0; i < arguments.size(); i++) {
                var arg = arguments.get(i);
                name.append(arg.getName()).append(":").append(arg.getType().getName());

                if(i != arguments.size() - 1) {
                    name.append(", ");
                }
            }
            name.append(")");

            editor.addMethodField(new Pair<>(
                    name.toString(),
                    new Pair<>(
                            method.getType().getName(),
                            method.getAccessModifier().getName()
                    )
            ));
        }
    }

    public void setEditing(boolean editing) {
        this.editing = editing;
    }

    public UMLClass getUmlClass() {
        return umlClass;
    }

    public SidebarClassEditor getEditor() {
        return editor;
    }

    public void setUmlClass(UMLClass umlClass) {
        this.umlClass = umlClass;
    }

    private void editClassAction(ActionEvent e) {
        switch(e.getActionCommand()) {
            case "Apply":
                if(editing) {
                    callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "Apply"));
                } else {
                    callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "Create"));
                }

                editor.clear();
                break;
            case "Delete":
                callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "Delete"));

                editor.clear();
                break;
            case "Cancel":
                editor.clear();
                callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "Revalidate"));
                break;
            case "Revalidate":
                callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "Revalidate"));
                break;
        }
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
