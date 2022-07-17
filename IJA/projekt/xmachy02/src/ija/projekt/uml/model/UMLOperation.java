/**
 * @file UMLOperation.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Operation (method) model
 *
 */

package ija.projekt.uml.model;

import ija.projekt.uml.model.enums.UMLAccessModifier;

import java.util.ArrayList;
import java.util.List;

public class UMLOperation extends UMLAttribute {
    private final ArrayList<UMLAttribute> arguments = new ArrayList<>();

    public UMLOperation(String name, UMLClassifier type) {
        super(name, type);
    }

    public UMLOperation(String name, UMLClassifier type, UMLAccessModifier accessModifier) {
        super(name, type, accessModifier);
    }

    public boolean addArgument(UMLAttribute arg) {
        return arguments.add(arg);
    }

    public static UMLOperation create(String name, UMLClassifier type, UMLAttribute... args) {
        UMLOperation op = new UMLOperation(name, type);
        for(UMLAttribute arg : args) {
            op.addArgument(arg);
        }
        return op;
    }

    public List<UMLAttribute> getArguments() {
        return arguments;
    }

    @Override
    public String toString() {
        return getName();
    }
}
