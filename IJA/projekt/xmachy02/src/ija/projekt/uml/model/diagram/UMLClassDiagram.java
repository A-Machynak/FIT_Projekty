/**
 * @file UMLClassDiagram.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Class diagram model
 *
 */

package ija.projekt.uml.model.diagram;

import ija.projekt.uml.model.UMLClass;
import ija.projekt.uml.model.UMLConnection;
import ija.projekt.uml.model.UMLElement;
import ija.projekt.uml.model.enums.UMLRelationship;

import java.util.ArrayList;

public class UMLClassDiagram extends UMLElement {
    private ArrayList<UMLConnection<UMLClass>> connections = new ArrayList<>();
    private ArrayList<UMLClass> classes = new ArrayList<>();

    public UMLClassDiagram(String name) {
        super(name);
    }

    public UMLConnection<UMLClass> addConnection(UMLClass first, UMLClass second, UMLRelationship relationship) {
        UMLConnection<UMLClass> c = new UMLConnection<>("", first, second);
        c.setRelationship(relationship);

        return c;
    }

    public UMLClass addClass(UMLClass umlClass) {
        for(UMLClass c : classes) {
            if(c.getName().equals(umlClass.getName())) {
                return null;
            }
        }
        classes.add(umlClass);
        return umlClass;
    }

    public boolean removeClass(UMLClass umlClass) {
        return classes.remove(umlClass);
    }

    public ArrayList<UMLConnection<UMLClass>> getConnections() {
        return connections;
    }

    public ArrayList<UMLClass> getClasses() {
        return classes;
    }
}
