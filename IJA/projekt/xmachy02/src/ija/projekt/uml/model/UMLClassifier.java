/**
 * @file UMLClassifier.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief UML diagram classifier model
 *
 */

package ija.projekt.uml.model;

/**
 * Reprezentuje klasifikátor v diagramu.
 * Odvozené třídy reprezentují konkrétní podoby klasifikátoru (třída, rozhraní, atribut, apod.)
 */
public class UMLClassifier extends UMLElement {
    private boolean isUserDefined = true;

    public UMLClassifier(String name) {
        super(name);
    }

    public UMLClassifier(String name, boolean isUserDefined) {
        super(name);
        this.isUserDefined = isUserDefined;
    }

    public static UMLClassifier forName(String name) {
        return new UMLClassifier(name, false);
    }

    public boolean isUserDefined() {
        return isUserDefined;
    }

    @Override
    public String toString() {
        return "UMLClassifier{" +
                "isUserDefined=" + isUserDefined +
                '}';
    }
}
