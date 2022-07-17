/**
 * @file UMLAttribute.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief UML attribute model
 *
 */

package ija.projekt.uml.model;

import ija.projekt.uml.model.enums.UMLAccessModifier;

/**
 * Reprezentuje atribut, který má své jméno a typ.
 * Typ atributu je reprezentován třidou UMLClassifier.
 * Lze použít jako atribut UML třídy nebo argument operace.
 */
public class UMLAttribute extends UMLElement {
    private final UMLClassifier type;
    private UMLAccessModifier accessModifier;

    public UMLAttribute(String name, UMLClassifier type) {
        super(name);
        this.type = type;
    }

    public UMLAttribute(String name, UMLClassifier type, UMLAccessModifier accessModifier) {
        super(name);
        this.type = type;
        this.accessModifier = accessModifier;
    }

    public UMLClassifier getType() {
        return type;
    }

    public UMLAccessModifier getAccessModifier() {
        return accessModifier;
    }

    public void setAccessModifier(UMLAccessModifier accessModifier) {
        this.accessModifier = accessModifier;
    }

    @Override
    public String toString() {
        return "UMLAttribute{" +
                "type=" + type +
                ", accessModifier=" + accessModifier +
                '}';
    }
}
