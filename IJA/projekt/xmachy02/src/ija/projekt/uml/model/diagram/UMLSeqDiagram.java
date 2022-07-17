/**
 * @file UMLSeqDiagram.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Sequential diagram's model
 *
 */

package ija.projekt.uml.model.diagram;

import ija.projekt.uml.model.*;
import ija.projekt.uml.model.enums.UMLRelationship;
import ija.projekt.uml.utils.Pair;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class UMLSeqDiagram extends UMLElement {
    private final ArrayList<UMLLifeline> lifelines = new ArrayList<>();
    private final ArrayList<UMLMessage> messages = new ArrayList<>();

    public UMLSeqDiagram(String name) {
        super(name);
    }

    public void addMessage(UMLMessage message) {
        messages.add(message);
    }

    public boolean removeMessage(UMLMessage message) {
        return messages.remove(message);
    }

    public ArrayList<UMLMessage> getMessages() {
        return messages;
    }

    public boolean addLifeline(UMLLifeline l) {
        return lifelines.add(l);
    }

    public void removeLifeline(UMLLifeline l) {
        lifelines.remove(l);
    }

    public List<UMLLifeline> getLifelines() {
        return Collections.unmodifiableList(lifelines);
    }
}

