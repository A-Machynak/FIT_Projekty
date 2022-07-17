/**
 * @file UMLClass.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Class diagram's class model
 *
 */

package ija.projekt.uml.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class UMLClass extends UMLClassifier {
    private boolean isAbstract = false;
    private final ArrayList<UMLAttribute> attributes = new ArrayList<>();
    private final ArrayList<UMLOperation> methods = new ArrayList<>();

    public UMLClass(String name) {
        super(name);
    }

    public boolean addAttribute(UMLAttribute attr) {
        return attributes.add(attr);
    }

    public int getAttrPosition(UMLAttribute attr) {
        return attributes.indexOf(attr);
    }

    public UMLAttribute removeAttrAtPosition(int pos) {
        return attributes.remove(pos);
    }

    public int moveAttrAtPosition(UMLAttribute attr, int pos) {
        int attrPos = getAttrPosition(attr);
        if(attrPos == -1) {
            return -1;
        }
        if(attrPos == pos) {
            return 0;
        }

        // save it to temporary
        UMLAttribute temp = attributes.get(attrPos);
        // remove it - shifts the list
        attributes.remove(attrPos);
        // add it to the new position - shifts everything else right
        attributes.add(pos, temp);
        return 0;
    }

    public List<UMLAttribute> getAttributes() {
        return Collections.unmodifiableList(attributes);
    }

    public void removeAttributes() {
        attributes.clear();
    }

    public void removeMethods() {
        methods.clear();
    }

    public boolean addMethod(UMLOperation method) {
        return methods.add(method);
    }

    public List<UMLOperation> getMethods() { return Collections.unmodifiableList(methods); }

    public boolean isAbstract() {
        return isAbstract;
    }

    public void setAbstract(boolean isAbstract) {
        this.isAbstract = isAbstract;
    }

    @Override
    public String toString() {
        return "UMLClass{" +
                "isAbstract=" + isAbstract +
                ", attributes=" + attributes +
                ", methods=" + methods +
                '}';
    }
}