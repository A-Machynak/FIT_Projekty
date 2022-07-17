package ija.projekt.uml.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class UMLLifeline extends UMLElement {
    private final UMLClass umlClass;
    private final ArrayList<UMLFocusOfControl> focs = new ArrayList<>();

    public UMLLifeline(UMLClass umlClass) {
        this.umlClass = umlClass;
    }

    public UMLLifeline(String name, UMLClass umlClass) {
        super(name);
        this.umlClass = umlClass;
    }

    public UMLClass getUmlClass() {
        return umlClass;
    }

    public void addFOC(UMLFocusOfControl foc) {
        focs.add(foc);
    }

    public boolean removeFOC(UMLFocusOfControl foc) {
        return focs.remove(foc);
    }

    public List<UMLFocusOfControl> getFocs() {
        return Collections.unmodifiableList(focs);
    }

    /**
     * Focus of control holds messages sent between objects
     */
    public static class UMLFocusOfControl extends UMLElement {
        //private final ArrayList<UMLMessage> messages = new ArrayList<>();

        public UMLFocusOfControl() {
            super("FOC");
        }

        /*public void addMessage(UMLMessage message) {
            messages.add(message);
        }*/

        /*public boolean removeMessage(UMLMessage message) {
            return messages.remove(message);
        }*/
    }
}
