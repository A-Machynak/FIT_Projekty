/**
 * @file UMLConnection.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief UML relationship between 2 elements
 *
 */

package ija.projekt.uml.model;

import ija.projekt.uml.model.enums.UMLRelationship;

public class UMLConnection<T extends UMLElement> extends UMLElement {
    private final T first;
    private final T second;
    private UMLRelationship relationship;

    public UMLConnection(String name, T first, T second) {
        super(name);
        this.first = first;
        this.second = second;
    }

    public T getFirst() {
        return first;
    }

    public T getSecond() {
        return second;
    }

    public UMLRelationship getRelationship() {
        return relationship;
    }

    public void setRelationship(UMLRelationship relationship) {
        this.relationship = relationship;
    }
}
