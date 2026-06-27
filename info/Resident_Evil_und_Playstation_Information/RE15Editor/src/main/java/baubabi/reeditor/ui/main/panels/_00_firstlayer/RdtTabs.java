package baubabi.reeditor.ui.main.panels._00_firstlayer;

import baubabi.reeditor.core.general.utils.FileUtilities;
import baubabi.reeditor.core.image.data.Image;
import baubabi.reeditor.core.rdt.RdtProcessing;
import baubabi.reeditor.ui.main.panels._01_secondlayer.SubTypeTab;
import lombok.Getter;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.File;

@Getter
public class RdtTabs {
    JPanel rdtPane = new JPanel();

    private final Image image;

    private final FileUtilities fileUtilities;

    SubTypeTab subTypeTab;

    public RdtTabs() {
        image = Image.getInstance();

        fileUtilities = FileUtilities.getInstance();

        String[] rdtComboBoxElements = new String[image.getImageRdtDirectories().size()];

        for (int i = 0; i < image.getImageRdtDirectories().size(); i++) {
            rdtComboBoxElements[i] = image.getImageRdtDirectories().get(i).getName();
            System.out.println("Combo box Elements: " + rdtComboBoxElements[i]);
        }

        JComboBox jComboBox = new JComboBox(rdtComboBoxElements);
        rdtPane.setLayout(new GridBagLayout());
        GridBagConstraints c = new GridBagConstraints();
        c.gridy = 0;
        rdtPane.add(jComboBox, c);
        jComboBox.addItemListener(new ItemListener() {
            public void itemStateChanged(ItemEvent arg0) {
                //Always gives a "deselected" (2) and a "selected" (1) event, the selected one is the one we are interested in.
                if(arg0.getStateChange() == 1) {
                    //Delete currently created Rdt Processing
                    RdtProcessing.getInstance().clearImageInstance();

                    //Delete Images before processing new ones
                    File bssDirectory = new File(image.getWorkDirectory() + File.separator + "BSS2BMP");
                    File[] imageBmpFiles = FileUtilities.getInstance().searchForFilesInDirectoryWithWildcard(bssDirectory, "*.bmp");
                    File[] imageBssFiles = FileUtilities.getInstance().searchForFilesInDirectoryWithWildcard(bssDirectory, "*.BSS");

                    for(int i = 0; i < imageBmpFiles.length; i++) {
                        FileUtilities.getInstance().removeFileFromDirectory(imageBmpFiles[i]);
                    }
                    for(int i = 0; i < imageBssFiles.length; i++) {
                        FileUtilities.getInstance().removeFileFromDirectory(imageBssFiles[i]);
                    }

                    //Create a new rdtProcessing instance
                    RdtProcessing.getInstance().getInstance();

                    //Get the new selected RDT item from the combobox
                    JComboBox selectedComboBox = (JComboBox) arg0.getSource();
                    String selectedRdtItem = (String) selectedComboBox.getSelectedItem();

                    //Fetch Rdt to the String
                    for(int i = 0; i < image.getImageRdtDirectories().size(); i++) {
                        if(image.getImageRdtDirectories().get(i).getName().equals(selectedRdtItem)) {
                            RdtProcessing.getInstance().parseRdt(image.getImageRdtDirectories().get(i));
                            break;
                        }
                    }

                    RdtProcessing.getInstance().parseScdElements();

                    rdtPane.remove(subTypeTab.getTabbedPane());

                    subTypeTab = new SubTypeTab(jComboBox.getSelectedItem().toString());

                    GridBagConstraints c = new GridBagConstraints();
                    c.gridy = 0;
                    c = new GridBagConstraints();
                    c.fill = GridBagConstraints.BOTH;
                    c.weightx = 1;
                    c.weighty = 1;
                    c.gridy = 1;

                    rdtPane.add(subTypeTab.getTabbedPane(), c);
                    rdtPane.updateUI();
                }
            }
        });

        c = new GridBagConstraints();
        c.fill = GridBagConstraints.BOTH;
        c.weightx = 1;
        c.weighty = 1;
        c.gridy = 1;

        //Adds next Layer of tabs
        subTypeTab = new SubTypeTab(jComboBox.getSelectedItem().toString());
        rdtPane.add(subTypeTab.getTabbedPane(), c);
    }
}
