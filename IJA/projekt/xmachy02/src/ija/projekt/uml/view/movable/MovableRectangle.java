/**
 * @file MovableRectangle.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Movable rectangle split into 3 rows
 *
 */

package ija.projekt.uml.view.movable;

import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;
import java.awt.*;

/**
 * Rectangle with 3 separated rows for text
 */
public class MovableRectangle extends MovableEntity {
    /**
     * Label for name
     */
    protected JLabel topLabel;

    /**
     * Label for attributes
     */
    protected JLabel midLabel;

    /**
     * Label for methods
     */
    protected JLabel botLabel;

    protected static final int BASE_FONT_SIZE = 12;

    protected GridBagConstraints gbc;

    public MovableRectangle() {
        createLayout();
    }

    public MovableRectangle(Point position) {
        super(position);
        createLayout();
    }

    private void createLayout() {
        setLayout(new GridBagLayout());

        gbc = new GridBagConstraints();
        gbc.weightx = 1;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridwidth = GridBagConstraints.REMAINDER;

        topLabel = new JLabel(" ");
        midLabel = new JLabel(" ");
        botLabel = new JLabel(" ");

        add(topLabel, gbc);
        add(midLabel, gbc);
        add(botLabel, gbc);

        Border lineBorder = BorderFactory.createLineBorder(Color.BLACK, 1);
        Border marginBorder = new EmptyBorder(2, 10, 2, 10);

        topLabel.setBorder(BorderFactory.createCompoundBorder(lineBorder, marginBorder));
        midLabel.setBorder(BorderFactory.createCompoundBorder(lineBorder, marginBorder));
        botLabel.setBorder(BorderFactory.createCompoundBorder(lineBorder, marginBorder));
    }

    public void setTopText(String text) {
        topLabel.setText(text);
    }

    public void setMidText(String text) {
        midLabel.setText(text);
    }

    public void setBotText(String text) {
        botLabel.setText(text);
    }

    public void showTopText(boolean show) {
        if(show) {
            // check if this label is already enabled and add it if not
            addIfNotAlready(topLabel);
        } else {
            remove(topLabel);
        }
    }

    public void showMidText(boolean show) {
        if(show) {
            // check if this label is already enabled and add it if not
            addIfNotAlready(midLabel);
        } else {
            remove(midLabel);
        }
    }

    public void showBotText(boolean show) {
        if(show) {
            // check if this label is already enabled and add it if not
            addIfNotAlready(botLabel);
        } else {
            remove(botLabel);
        }
    }

    private void addIfNotAlready(Container container) {
        for(Component c : getComponents()) {
            if(c.equals(container)) {
                return;
            }
        }
        add(container);
    }

    /**
     * Scaling by setting the font size
     * @param factor scaling factor
     */
    @Override
    public void scale(float factor) {
        /*Font newFont = new Font(topLabel.getFont().getName(), Font.PLAIN, (int)(BASE_FONT_SIZE * factor));
        topLabel.setFont(newFont);
        midLabel.setFont(newFont);
        botLabel.setFont(newFont);*/
    }
}
