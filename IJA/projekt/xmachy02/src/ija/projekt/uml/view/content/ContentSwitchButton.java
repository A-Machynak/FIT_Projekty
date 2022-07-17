package ija.projekt.uml.view.content;

import javax.swing.*;
import javax.swing.event.MouseInputListener;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

/**
 * Button for switching content
 */
public class ContentSwitchButton extends JButton implements MouseInputListener {
    private final Container content;

    private boolean isEnabled = true;
    private boolean isContentSwitched = false;

    public ContentSwitchButton(Container content, boolean enablePopupMenu) {
        this.content = content;

        setText("Tab");
        addMouseListener(this);
        setBorderPainted(false);

        setBackground(Color.gray);
        setForeground(Color.white);

        if(enablePopupMenu) {
            JPopupMenu popupMenu = new JPopupMenu();
            JMenuItem itemEdit = new JMenuItem("Change name");
            itemEdit.addActionListener(e -> nameChangeAction());
            JMenuItem itemDelete = new JMenuItem("Delete");
            itemDelete.addActionListener(e -> deleteConfirmationAction());
            popupMenu.add(itemEdit);
            popupMenu.add(itemDelete);
            addMouseListener(new PopupListener(popupMenu));
        }
    }

    private void nameChangeAction() {
        String inputValue = JOptionPane.showInputDialog("Select new tab name", getText());
        setText(inputValue);
    }

    private void deleteConfirmationAction() {
        int retValue = JOptionPane.showConfirmDialog(
                this,
                "Are you sure?",
                "Delete confirmation",
                JOptionPane.OK_CANCEL_OPTION);
        if(retValue == 0) {
            fireActionPerformed(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "Delete"));
        }
    }

    /**
     * Set whether clicking this button switches content in main window
     * @param isEnabled enable/disable
     */
    public void enableContentSwitch(boolean isEnabled) {
        this.isEnabled = isEnabled;
    }

    /**
     * Simulate left click to fire switch content event
     */
    public void switchContent() {
        this.mouseClicked(new MouseEvent(this, MouseEvent.MOUSE_CLICKED, 0, 0, 0, 0, 1,
                false, MouseEvent.BUTTON1));
    }

    /**
     * Return button to the "non-switched" state
     */
    public void unswitch() {
        isContentSwitched = false;
        setBackground(Color.gray);
    }

    /**
     * Content getter
     * @return content
     */
    public Container getContent() {
        return content;
    }

    /**
     * Is content currently switched in main window
     * @return content currently switched
     */
    public boolean isContentSwitched() {
        return isContentSwitched;
    }

    @Override
    public void mouseClicked(MouseEvent e) {
        if(!isEnabled) {
            return;
        }

        if(e.getButton() == MouseEvent.BUTTON1) {
            if(isContentSwitched) {
                return;
            }

            fireActionPerformed(new ActionEvent(content, ActionEvent.ACTION_PERFORMED, "Switch"));
            setBackground(Color.darkGray);
            isContentSwitched = true;
        }
    }

    @Override
    public void mouseEntered(MouseEvent e) {
        if(isContentSwitched) {
            return;
        }

        setBackground(Color.BLACK);
    }

    @Override
    public void mouseExited(MouseEvent e) {
        if(isContentSwitched) {
            return;
        }

        setBackground(Color.gray);
    }

    @Override
    public void mousePressed(MouseEvent e) {
        /// Intentionally empty
    }

    @Override
    public void mouseReleased(MouseEvent e) {
        /// Intentionally empty
    }

    @Override
    public void mouseDragged(MouseEvent e) {
        /// Intentionally empty
    }

    @Override
    public void mouseMoved(MouseEvent e) {
        /// Intentionally empty
    }

    class PopupListener extends MouseAdapter {
        JPopupMenu popup;

        PopupListener(JPopupMenu popupMenu) {
            popup = popupMenu;
        }

        public void mousePressed(MouseEvent e) {
            maybeShowPopup(e);
        }

        public void mouseReleased(MouseEvent e) {
            maybeShowPopup(e);
        }

        private void maybeShowPopup(MouseEvent e) {
            if (e.isPopupTrigger()) {
                popup.show(e.getComponent(),
                        e.getX(), e.getY());
            }
        }
    }
}
