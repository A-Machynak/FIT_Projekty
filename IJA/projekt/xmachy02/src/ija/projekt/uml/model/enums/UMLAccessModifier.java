/**
 * @file UMLAccessModifier.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Access modifiers
 *
 */

package ija.projekt.uml.model.enums;

import java.io.Serializable;

/**
 * The enum Uml access modifier.
 */
public enum UMLAccessModifier implements Serializable {
    /**
     * Public uml access modifier.
     */
    PUBLIC("public", "+"),
    /**
     * Private uml access modifier.
     */
    PRIVATE("private", "-"),
    /**
     * Protected uml access modifier.
     */
    PROTECTED("protected", "#"),
    /**
     * Package uml access modifier.
     */
    PACKAGE("package", "~"),
    /**
     * None uml access modifier.
     */
    NONE("", "");

    private final String name;
    private final String symbol;

    UMLAccessModifier(String name, String symbol) {
        this.name = name;
        this.symbol = symbol;
    }

    /**
     * Gets name.
     *
     * @return the name
     */
    public String getName() {
        return this.name;
    }

    /**
     * Gets symbol.
     *
     * @return the symbol
     */
    public String getSymbol() {
        return symbol;
    }

    /**
     * Convert string to UMLAccessModifier.
     *
     * @param nameOrSymbol the name or symbol
     * @return the uml access modifier
     */
    public static UMLAccessModifier toUMLAccessModifier(String nameOrSymbol) {
        for(UMLAccessModifier m : UMLAccessModifier.values()) {
            if(m.getName().equals(nameOrSymbol) || m.getSymbol().equals(nameOrSymbol)) return m;
        }
        return NONE;
    }
}
