/**
 * @file MainWindow.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Main window
 *
 */

package ija.projekt.uml.view;

import ija.projekt.uml.controller.MainController;
import ija.projekt.uml.model.UMLClass;

import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;
import java.awt.*;

public class MainWindow {
    private final JFrame mainFrame;
    private final Container mainContainer;

    private BorderLayout mainLayout;
    private MainController mainController;

    private JMenuBar topMenuBar;

    private Container topContainer;
    private Container centerContainer;
    private Container bottomContainer; // For info text

    private JLabel infoTextLabel;

    private Dimension screenSize;

    private static final int MINIMUM_SIZE_WIDTH = 400;
    private static final int MINIMUM_SIZE_HEIGHT = 200;
    private static final int MAINLAYOUT_HGAP = 5;
    private static final int MAINLAYOUT_VGAP = 5;

    public MainWindow(JFrame mainFrame) {
        this.mainFrame = mainFrame;
        this.mainContainer = this.mainFrame.getContentPane();

        init();
        createTopMenu();
        this.mainController = new MainController(this);
        //showFeatureWindow();
        //TEST();
    }

    private void init() {
        mainLayout = new BorderLayout(MAINLAYOUT_HGAP, MAINLAYOUT_VGAP);

        screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        mainFrame.setLayout(mainLayout);
        mainFrame.setMinimumSize(new Dimension(MINIMUM_SIZE_WIDTH, MINIMUM_SIZE_HEIGHT));
        mainFrame.setSize(screenSize.width / 2, screenSize.height / 2);

        /// Create top, bottom and center containers
        topContainer = new JPanel();
        BoxLayout topLayout = new BoxLayout(topContainer, BoxLayout.X_AXIS);
        topContainer.setLayout(topLayout);
        centerContainer = new JPanel(new BorderLayout());
        bottomContainer = new JPanel(new BorderLayout());

        JScrollPane topScrollPane = new JScrollPane(topContainer,
                ScrollPaneConstants.VERTICAL_SCROLLBAR_NEVER,
                ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        topScrollPane.getViewport().addChangeListener(e -> {
            if(topScrollPane.getHorizontalScrollBar().isVisible()) {
                topScrollPane.setPreferredSize(new Dimension(0, 40));
            } else {
                topScrollPane.setPreferredSize(topScrollPane.getMinimumSize());
            }
            revalidate();
        });

        mainContainer.add(topScrollPane, BorderLayout.PAGE_START);
        mainContainer.add(centerContainer, BorderLayout.CENTER);
        mainContainer.add(bottomContainer, BorderLayout.PAGE_END);

        // Initialize bottom info bar
        infoTextLabel = new JLabel("Welcome! Click on \"Help\" -> \"Controls\" for tips");
        Border compoundBorder = BorderFactory.createCompoundBorder(
                BorderFactory.createRaisedBevelBorder(),
                BorderFactory.createLoweredBevelBorder());
        Border marginBorder = new EmptyBorder(2, 10, 2, 10);
        infoTextLabel.setBorder(BorderFactory.createCompoundBorder(compoundBorder, marginBorder));

        bottomContainer.add(infoTextLabel, BorderLayout.CENTER);
    }

    private void TEST() {
        mainController.addUMLClass(new UMLClass("Test"));
    }

    private void createTopMenu() {
        topMenuBar = new JMenuBar();

        /// "File" menu
        JMenu fileMenu = new JMenu("File");
        JMenu settingsMenu = new JMenu("Settings");
        JMenu helpMenu = new JMenu("Help");

        // "File" -> "New file", "Load..." and "Save" buttons
        JMenuItem fileMenuItemNew = new JMenuItem("New file");
        fileMenuItemNew.addActionListener(e -> newFileButtonAction());
        JMenuItem fileMenuItemLoad = new JMenuItem("Load...");
        fileMenuItemLoad.addActionListener(e -> loadFileButtonAction());
        JMenuItem fileMenuItemSave = new JMenuItem("Save");
        fileMenuItemSave.addActionListener(e -> saveFileButtonAction());

        fileMenu.add(fileMenuItemNew);
        fileMenu.add(fileMenuItemLoad);
        fileMenu.add(fileMenuItemSave);

        // "Help" -> "Features" and "About" buttons
        JMenuItem helpMenuItemFeatures = new JMenuItem("Features");
        helpMenuItemFeatures.addActionListener(e -> featuresHelpButtonAction());
        JMenuItem helpMenuItemAbout = new JMenuItem("About");
        helpMenuItemAbout.addActionListener(e -> aboutHelpButtonAction());

        helpMenu.add(helpMenuItemFeatures);
        helpMenu.add(helpMenuItemAbout);

        topMenuBar.add(fileMenu);
        topMenuBar.add(settingsMenu);
        topMenuBar.add(helpMenu);

        mainFrame.setJMenuBar(topMenuBar);
    }

    public void setCenterContainer(Container container) {
        centerContainer.removeAll();
        if(container != null) {
            centerContainer.add(container, BorderLayout.CENTER);
        }
        revalidate();
    }

    public void addToTopContainer(Container container) {
        topContainer.add(container);
    }

    public void removeFromTopContainer(Component container) {
        for(Component c : topContainer.getComponents()) {
            if(c.equals(container)) {
                topContainer.remove(c);
                return;
            }
        }
    }

    public void setInfoText(String text) {
        infoTextLabel.setText(text);
    }

    private void newFileButtonAction() {
        setInfoText("File - Create new file");

    }

    private void loadFileButtonAction() {
        setInfoText("File - Load a file");

    }

    private void saveFileButtonAction() {
        setInfoText("File - Save a file");

    }

    private void featuresHelpButtonAction() {
        setInfoText("Help - Features");
        showFeatureWindow();
    }

    private void aboutHelpButtonAction() {
        setInfoText("Help - About");
        String msg = "UML editor.\nMade by Machynak Augustin";

        JOptionPane.showMessageDialog(mainFrame, msg, "Info", JOptionPane.INFORMATION_MESSAGE);
    }

    public void revalidate() {
        mainFrame.revalidate();
        mainFrame.repaint();
    }

    public void errorDialog(String message) {
        JOptionPane.showMessageDialog(mainFrame, message, "Error", JOptionPane.ERROR_MESSAGE);
    }

    private void showFeatureWindow() {
        String msg = "Currently available features:\n";
        msg += "Add class on canvas (Left click on canvas after selecting \"Class\")\n";
        msg += "Move class on canvas (Left click and drag class)\n";
        msg += "Create a relationship between classes (Left click on 2 classes after selecting a relationship)\n";
        msg += "Move canvas (Hold middle mouse button and drag)\n";
        msg += "Add new sequence diagram (Click on \"+\" button next to the \"class\" tab)\n";
        msg += "Delete or rename sequence diagram (Right click on sequence diagram tab to bring up popup menu)\n";
        msg += "\nThis window can be accessed in tab Help - Features";

        JOptionPane.showMessageDialog(mainFrame, msg, "Features", JOptionPane.INFORMATION_MESSAGE);
    }
}
