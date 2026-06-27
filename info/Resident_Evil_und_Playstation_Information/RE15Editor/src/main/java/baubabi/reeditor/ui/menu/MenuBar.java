package baubabi.reeditor.ui.menu;

import baubabi.reeditor.core.general.utils.FileUtilities;
import baubabi.reeditor.core.general.utils.ProcessUtilities;
import baubabi.reeditor.core.general.utils.UserConfigurationUtility;
import baubabi.reeditor.core.image.data.Image;
import baubabi.reeditor.core.rdt.RdtProcessing;
import baubabi.reeditor.ui.menu.about.AboutWindow;
import baubabi.reeditor.ui.menu.load.LoadUtilities;
import baubabi.reeditor.ui.menu.load.LoadWindow;
import baubabi.reeditor.ui.menu.thanks.ThanksWindow;
import lombok.Getter;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.File;

@Getter
public class MenuBar {

    private final JMenuBar menuBar;

    private Image image;

    private final LoadUtilities loadUtilities;

    private final PropertyChangeSupport changes = new PropertyChangeSupport(this);


    public MenuBar() {
        loadUtilities = LoadUtilities.getInstance();

        //Create the menu bar.
        menuBar = new JMenuBar();

        //First Menu Bar - "File"
        JMenu fileMenu = new JMenu("File");

        //"Image" as first Submenu of "File"
        JMenu imageMenu = new JMenu("Image");
        imageMenu.setToolTipText("Image needs to be selected in the beginning");

        //Items of "Image"
        JMenuItem loadImage = new JMenuItem("Load");
        imageMenu.add(loadImage);

        JMenuItem saveImage = new JMenuItem("Save");
        saveImage.setAction(createSaveShortcut());
        imageMenu.add(saveImage);

        //Add Image Sub Menu to File
        fileMenu.add(imageMenu);

        //"Close" possible via menu
        JMenuItem exitItem = new JMenuItem("Exit");
        fileMenu.add(exitItem);

        //Add File Menu to Menu Bar
        menuBar.add(fileMenu);

        //Help Menu
        JMenu helpMenu = new JMenu("Help");

        //About and Thanks Menu
        JMenuItem aboutItem = new JMenuItem("About");
        helpMenu.add(aboutItem);
        JMenuItem thanksItem = new JMenuItem("Thanks");
        helpMenu.add(thanksItem);

        menuBar.add(helpMenu);

        //Emulator Menu
        JMenu emuMenu = new JMenu("Emulator");

        //Select Emulator and run Emulation Menu
        JMenuItem selectEmulatorItem = new JMenuItem("Select ePSXe");
        emuMenu.add(selectEmulatorItem);
        JMenuItem runOnEmulatorItem = new JMenuItem("Run on ePSXe");
        emuMenu.add(runOnEmulatorItem);

        menuBar.add(emuMenu);

        initializeListeners(loadImage, aboutItem, thanksItem, exitItem, selectEmulatorItem, runOnEmulatorItem);
    }

    private void initializeListeners(JMenuItem loadImage, JMenuItem aboutItem, JMenuItem thanksItem, JMenuItem exitItem, JMenuItem selectEmulatorItem, JMenuItem runOnEmulatorItem) {
        loadImage.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                new LoadWindow();

                //If image was selected
                image = Image.getInstance();
                if (image != null) {

                    //Initialize the ImageUtility helper class
                    loadUtilities.initializeImageUtilities();

                    //Extract the selected RE 1.5 Image
                    loadUtilities.extractImage();

                    //Loads the RDT Elements
                    loadUtilities.loadRdtElements();

                    RdtProcessing rdtProcessing = RdtProcessing.getInstance();

                    //With the "dataLoaded" property, it continues at the StartAndRdtUI class with creating the RDT tabs.
                    changes.firePropertyChange("dataLoaded", null, rdtProcessing.getAllScdDataElements());
                } else {
                    System.out.println("IMAGE NOT SET");
                }
            }
        });

        aboutItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                new AboutWindow();
            }
        });
        thanksItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                new ThanksWindow();
            }
        });
        exitItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                System.exit(0);
            }
        });

        selectEmulatorItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                JFrame frame = new JFrame("load Image");
                JFileChooser fileChooser = new JFileChooser();
                fileChooser.setAcceptAllFileFilterUsed(false);
                fileChooser.setCurrentDirectory(new java.io.File("."));
                fileChooser.setSelectedFile(new File(""));
                fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
                if (fileChooser.showOpenDialog(frame) == JFileChooser.OPEN_DIALOG) {
                    UserConfigurationUtility.getInstance().saveEPSXeConfiguration(fileChooser.getSelectedFile());
                    frame.dispose();
                } else {
                    frame.dispose();
                }
            }
        });

        runOnEmulatorItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                ProcessUtilities processUtilities = ProcessUtilities.getInstance();
                File file = new File(image.getWorkDirectory() + File.separator + image.getImageName().replace(".bin", "") + "_modified.bin");
                File fileNew = new File(image.getWorkDirectory() + File.separator + "play.bin");

                FileUtilities fileUtilities = FileUtilities.getInstance();
                fileUtilities.copyFile(file.getAbsolutePath(), fileNew.getAbsolutePath());

                processUtilities.startEmulator(fileNew);
            }
        });
    }

    private Action createSaveShortcut() {
        Action saveAction = new AbstractAction("Save") {
            @Override
            public void actionPerformed(ActionEvent e) {
                changes.firePropertyChange("savePressed", null, "savePressed");
            }
        };

        saveAction.putValue(Action.ACCELERATOR_KEY,
                KeyStroke.getKeyStroke(KeyEvent.VK_S, KeyEvent.CTRL_DOWN_MASK));

        return saveAction;
    }

    public void addPropertyChangeListener(PropertyChangeListener l) {
        changes.addPropertyChangeListener(l);
    }

    public void removePropertyChangeListener(PropertyChangeListener l) {
        changes.removePropertyChangeListener(l);
    }
}