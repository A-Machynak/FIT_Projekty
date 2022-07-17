/**
 * @file SidebarClassEditor.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Class editor view
 *
 */

package ija.projekt.uml.view.content;

import ija.projekt.uml.utils.Pair;

import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;
import javax.swing.event.EventListenerList;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

public class SidebarClassEditor extends JPanel {
    public enum SidebarEditorCommands {
        APPLY("Apply"),
        DELETE("Delete"),
        CANCEL("Cancel");

        private final String command;
        SidebarEditorCommands(String command) {
            this.command = command;
        }

        public String getCommand() { return command; }
    }

    private JPanel classPanel;
    private JPanel attributePanel;
    private JPanel methodPanel;
    private JPanel buttonPanel;

    private JScrollPane classScrollPane;
    private JScrollPane attributeScrollPane;
    private JScrollPane methodScrollPane;

    private JTabbedPane tabbedPane;

    private JButton applyButton;
    private JButton cancelButton;
    private JButton deleteButton;

    private JTextField classNameField;
    private ArrayList<SidebarClassEditorField> methodsFields = new ArrayList<>();
    private ArrayList<SidebarClassEditorField> attributesFields = new ArrayList<>();
    private EventListenerList listenerList = new EventListenerList();

    private static final int FONT_SIZE = 18;

    public SidebarClassEditor() {
        clear();
        recreateLayout();
    }

    public void recreateLayout() {
        clear();

        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        setBorder(new EmptyBorder(0, 10, 0, 10));

        tabbedPane = new JTabbedPane();

        classPanel = new JPanel();
        attributePanel = new JPanel();
        methodPanel = new JPanel();

        classPanel.setLayout(new BoxLayout(classPanel, BoxLayout.Y_AXIS));
        attributePanel.setLayout(new BoxLayout(attributePanel, BoxLayout.Y_AXIS));
        methodPanel.setLayout(new BoxLayout(methodPanel, BoxLayout.Y_AXIS));

        JLabel classNameLabel = new JLabel("Classifier details");
        JLabel attributeNameLabel = new JLabel("Attribute details");
        JLabel methodNameLabel = new JLabel("Operation details");

        customLabel(classNameLabel);
        customLabel(attributeNameLabel);
        customLabel(methodNameLabel);

        classPanel.add(classNameLabel);
        attributePanel.add(attributeNameLabel);
        methodPanel.add(methodNameLabel);

        JButton addMethodButton = new JButton("Add method");
        addMethodButton.setAlignmentX(Component.CENTER_ALIGNMENT);
        addMethodButton.addActionListener(e -> addMethodField(null));

        JButton addAttributeButton = new JButton("Add attribute");
        addAttributeButton.setAlignmentX(Component.CENTER_ALIGNMENT);
        addAttributeButton.addActionListener(e -> addAttributeField(null));

        methodPanel.add(addMethodButton);
        attributePanel.add(addAttributeButton);

        classNameField = new JTextField("ClassName");
        classNameField.setMaximumSize(new Dimension(Integer.MAX_VALUE, 20));
        classPanel.add(classNameField);

        classScrollPane = new JScrollPane(classPanel,
                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        attributeScrollPane = new JScrollPane(attributePanel,
                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        methodScrollPane = new JScrollPane(methodPanel,
                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);

        tabbedPane.addTab("Classifier", classScrollPane);
        tabbedPane.addTab("Attributes", attributeScrollPane);
        tabbedPane.addTab("Operations", methodScrollPane);

        buttonPanel = new JPanel();
        // Action buttons
        applyButton = new JButton("Apply");
        applyButton.addActionListener(e -> applyButtonAction());
        deleteButton = new JButton("Delete");
        deleteButton.addActionListener(e -> deleteButtonAction());
        cancelButton = new JButton("Cancel");
        cancelButton.addActionListener(e -> cancelButtonAction());

        buttonPanel.add(applyButton);
        buttonPanel.add(deleteButton);
        buttonPanel.add(cancelButton);

        add(tabbedPane);
        add(Box.createRigidArea(new Dimension(0, 5)));
        add(buttonPanel);
    }

    public void clear() {
        removeAll();
        revalidateAction();
    }

    public void reset() {
        recreateLayout();

        for(SidebarClassEditorField f : methodsFields) {
            methodPanel.remove(f);
        }
        methodsFields.clear();

        for(SidebarClassEditorField f : attributesFields) {
            attributePanel.remove(f);
        }
        attributesFields.clear();
        revalidateAction();
    }

    private void customLabel(JLabel label) {
        Border compoundBorder = BorderFactory.createCompoundBorder(
                BorderFactory.createRaisedBevelBorder(),
                BorderFactory.createLoweredBevelBorder());
        Border marginBorder = new EmptyBorder(2, 10, 2, 10);
        label.setBorder(BorderFactory.createCompoundBorder(compoundBorder, marginBorder));

        label.setFont(new Font(label.getFont().getName(), Font.PLAIN, FONT_SIZE));
        label.setAlignmentX(Component.CENTER_ALIGNMENT);
    }

    public void setClassNameField(String text) {
        classNameField.setText(text);
    }

    public void addMethodField(Pair<String, Pair<String, String>> method) {
        SidebarClassEditorField f = new SidebarClassEditorField();
        methodsFields.add(f);
        methodPanel.add(f);

        if(method != null) {
            f.setNameFieldText(method.getFirst());
            f.setTypeFieldText(method.getSecond().getFirst());
            f.setAccessModifier(method.getSecond().getSecond());
        }

        revalidateAction();
    }

    public void addAttributeField(Pair<String, Pair<String, String>> attr) {
        SidebarClassEditorField f = new SidebarClassEditorField();
        attributesFields.add(f);
        attributePanel.add(f);

        if(attr != null) {
            f.setNameFieldText(attr.getFirst());
            f.setTypeFieldText(attr.getSecond().getFirst());
            f.setAccessModifier(attr.getSecond().getSecond());
        }
        revalidateAction();
    }

    /**
     * Class name getter
     * @return class name
     */
    public String getName() {
        return classNameField.getText();
    }

    /**
     * Attributes getter
     * @return Arraylist of attributes in format: <AttributeName,<Type,AccessModifier>>
     */
    public ArrayList<Pair<String, Pair<String, String>>> getAttributes() {
        // < Attribute name, < Type, Access modifier >>
        ArrayList<Pair<String, Pair<String, String>>> arr = new ArrayList<>();
        for(SidebarClassEditorField attr : attributesFields) {
            arr.add(new Pair<>(
                    attr.getNameFieldText(),
                    new Pair<>(
                            attr.getTypeFieldText(),
                            attr.getAccessModifier()
                    )));
        }
        return arr;
    }

    /**
     * Methods getter
     * @return Arraylist of methods in format: <MethodName,<ReturnType,AccessModifier>>
     */
    public ArrayList<Pair<String, Pair<String, String>>> getMethods() {
        // < Method name, < Return type, Access modifier >>
        ArrayList<Pair<String, Pair<String, String>>> arr = new ArrayList<>();
        for(SidebarClassEditorField method : methodsFields) {
            arr.add(new Pair<>(
                    method.getNameFieldText(),
                    new Pair<>(
                            method.getTypeFieldText(),
                            method.getAccessModifier()
                    )));
        }
        return arr;
    }

    private void applyButtonAction() {
        callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, SidebarEditorCommands.APPLY.getCommand()));
    }

    private void cancelButtonAction() {
        callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, SidebarEditorCommands.CANCEL.getCommand()));
    }

    private void deleteButtonAction() {
        callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, SidebarEditorCommands.DELETE.getCommand()));
    }

    private void revalidateAction() {
        callListeners(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "Revalidate"));
    }

    public void addListener(ActionListener l) {
        listenerList.add(ActionListener.class, l);
    }

    private void callListeners(ActionEvent e) {
        for(ActionListener l : listenerList.getListeners(ActionListener.class)) {
            l.actionPerformed(e);
        }
    }

    class SidebarClassEditorField extends JPanel {
        private final JTextField nameField;
        private final JTextField typeField;

        private final ArrayList<JRadioButton> buttons;

        private final String[] accessModifiers = { "public", "private", "protected", "package" };

        SidebarClassEditorField() {
            setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
            setBorder(BorderFactory.createLoweredBevelBorder());

            JPanel topPanel = new JPanel();
            topPanel.setLayout(new BoxLayout(topPanel, BoxLayout.X_AXIS));

            JPanel botPanel = new JPanel();
            botPanel.setLayout(new BoxLayout(botPanel, BoxLayout.X_AXIS));

            nameField = new JTextField();
            nameField.setMaximumSize(new Dimension(Integer.MAX_VALUE, 20));
            nameField.setToolTipText("Name");

            typeField = new JTextField();
            typeField.setMaximumSize(new Dimension(Integer.MAX_VALUE, 20));
            typeField.setToolTipText("Type");

            topPanel.add(nameField);
            topPanel.add(typeField);

            ButtonGroup buttonGroup = new ButtonGroup();
            buttons = new ArrayList<>();

            add(topPanel);
            add(botPanel);

            for(String m : accessModifiers) {
                JRadioButton b = new JRadioButton(m);

                buttonGroup.add(b);
                buttons.add(b);
                botPanel.add(b);
                if(buttonGroup.getSelection() == null) {
                    b.setSelected(true);
                }
            }
        }

        public String getNameFieldText() {
            return nameField.getText();
        }

        public void setNameFieldText(String text) {
            nameField.setText(text);
        }

        public String getTypeFieldText() {
            return typeField.getText();
        }

        public void setTypeFieldText(String text) {
            typeField.setText(text);
        }

        public String getAccessModifier() {
            for(JRadioButton b : buttons) {
                if(b.isSelected()) {
                    return b.getText();
                }
            }
            return accessModifiers[0];
        }

        public void setAccessModifier(String modifier) {
            for(JRadioButton b : buttons) {
                if(b.getText().equals(modifier)) {
                    b.setSelected(true);
                    break;
                }
            }
        }
    }
}
