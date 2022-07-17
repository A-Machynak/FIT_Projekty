/**
 * @file ClassController.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief UMLClass's UI and model controller
 *
 */

package ija.projekt.uml.controller;

import ija.projekt.uml.model.UMLAttribute;
import ija.projekt.uml.model.UMLClass;
import ija.projekt.uml.model.UMLOperation;
import ija.projekt.uml.view.movable.MovableRectangle;

public class ClassController {
    private final UMLClass umlClass;
    private final MovableRectangle movableRectangle;

    public ClassController(UMLClass umlClass, MovableRectangle movableRectangle) {
        this.umlClass = umlClass;
        this.movableRectangle = movableRectangle;
    }

    public MovableRectangle getMovableUMLClass() {
        return movableRectangle;
    }

    public UMLClass getUmlClass() {
        return umlClass;
    }

    /**
     * Update UI based on UMLClass values
     */
    public void update() {
        // Update class name
        movableRectangle.setTopText(umlClass.getName());

        // Update attributes
        StringBuilder attrStr = new StringBuilder();

        for(UMLAttribute attribute : umlClass.getAttributes()) {
            // [UMLAccessModifier] [attributeName]
            attrStr.append(attribute.getAccessModifier().getSymbol())
                    .append(" ")
                    .append(attribute.getName());

            String typeName = attribute.getType().getName();
            if(!typeName.equals("")) {
                // : [attributeType]
                attrStr.append(": ")
                        .append(typeName);
            }
            attrStr.append("\n");
        }
        movableRectangle.setMidText(toHtml(attrStr.toString()));

        // Update methods
        StringBuilder methodStr = new StringBuilder();
        for(UMLOperation method : umlClass.getMethods()) {
            /// [UMLAccessModifier] [methodName]
            methodStr.append(method.getAccessModifier().getSymbol())
                    .append(" ")
                    .append(method.getName())
                    .append("(");

            /// ( [arg1] : [type1], ... )
            var arguments = method.getArguments();
            for(int i = 0; i < arguments.size(); i++) {
                var arg = arguments.get(i);
                methodStr.append(arg.getName()).append(":").append(arg.getType().getName());

                if(i != arguments.size() - 1) {
                    methodStr.append(", ");
                }
            }
            methodStr.append(")");
            if(!method.getType().getName().equals("")) {
                methodStr.append(": ").append(method.getType().getName());
            }
            methodStr.append("\n");
        }
        movableRectangle.setBotText(toHtml(methodStr.toString()));

        movableRectangle.revalidate();
        movableRectangle.repaint();
    }

    /**
     * Use html tags to create label newlines, since JLabel doesn't support newlines (\n)
     * @param text text to convert to html format
     * @return converted text
     */
    private String toHtml(String text) {
        // https://stackoverflow.com/questions/1090098/newline-in-jlabel
        return "<html>" + text
                .replaceAll("<", "&lt;")
                .replaceAll(">", "&gt;")
                .replaceAll("\n", "<br/>")
                + "</html>";
    }
}
