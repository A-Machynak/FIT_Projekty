/**
 * @file UMLRelationship.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Class diagram relationship types
 *
 */

package ija.projekt.uml.model.enums;

import java.io.Serializable;

public enum UMLRelationship implements Serializable {
    NONE("None"),
    // Relationships between classes in class diagram
    ASSOCIATION("Association"),
    AGGREGATION("Aggregation"),
    COMPOSITION("Composition"),
    GENERALIZATION("Generalization");

    private final String text;
    UMLRelationship(String text) {
        this.text = text;
    }
    public String getText() {
        return text;
    }

    public static UMLRelationship fromString(String s) {
        for(UMLRelationship r : UMLRelationship.values()) {
            if(r.text.equals(s)) {
                return r;
            }
        }
        return NONE;
    }
}
