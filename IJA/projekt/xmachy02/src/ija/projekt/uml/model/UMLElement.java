/**
 * @file UMLElement.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Basic element in UML diagram
 *
 */

package ija.projekt.uml.model;

import java.io.Serializable;

public class UMLElement implements Serializable {
    private String name;

    public UMLElement() {
        this.name = "";
    }

    public UMLElement(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}
