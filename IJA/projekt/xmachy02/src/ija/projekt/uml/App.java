/**
 * @file App.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Entry point for this project
 *
 */

package ija.projekt.uml;

import ija.projekt.uml.utils.FunctionParser;
import ija.projekt.uml.view.MainWindow;

import javax.swing.*;
import javax.swing.plaf.ColorUIResource;
import java.awt.*;

/**
 * Application entry point
 */
public class App {
    /**
     * The entry point of application.
     *
     * @param args the input arguments
     */
    public static void main(String[] args) {
        /*String[] badTests = { "x", "x(", "x(a", "x(a)", "x(a:)", "x(:a)", "x(a:b:c)" };
        String[] goodTests = { "x()", "x(a:b)", "test(testValue : int, testVal2 : String)" };

        for (String badTest : badTests) {
            System.out.println("Running: " + badTest);
            var f = FunctionParser.parseFunction(badTest, "public");
            System.out.println(" - Expected null, got: " + f);
        }

        for (String goodTest : goodTests) {
            System.out.println("Running: " + goodTest);
            var f = FunctionParser.parseFunction(goodTest, "public");
            System.out.println(" - Expected something, got: " + f);
        }*/


        try {
            UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
        } catch (ClassNotFoundException | InstantiationException | IllegalAccessException | UnsupportedLookAndFeelException e) {
            e.printStackTrace();
        }

        // Removes the dotted border around controls
        // Source: https://stackoverflow.com/questions/9361658/disable-jbutton-focus-border
        UIManager.put("Button.focus", new ColorUIResource(new Color(0, 0, 0, 0)));
        UIManager.put("ToggleButton.focus", new ColorUIResource(new Color(0, 0, 0, 0)));
        UIManager.put("RadioButton.focus", new ColorUIResource(new Color(0, 0, 0, 0)));

        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("IJA_UML");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

            frame.setVisible(true);
            frame.pack();


            new MainWindow(frame);
        });
    }
}
