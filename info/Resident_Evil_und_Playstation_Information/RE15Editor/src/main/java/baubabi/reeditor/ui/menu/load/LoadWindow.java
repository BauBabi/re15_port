package baubabi.reeditor.ui.menu.load;

import baubabi.reeditor.core.image.data.Image;
import lombok.Getter;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.io.File;

@Getter
public class LoadWindow {

    JFrame frame = new JFrame("load Image");

    public LoadWindow() {
        File imageFile;
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setAcceptAllFileFilterUsed(false);
        fileChooser.setFileFilter(new FileNameExtensionFilter("Resident Evil 1.5 Image .bin", "bin"));
        fileChooser.setCurrentDirectory(new java.io.File("."));
        fileChooser.setSelectedFile(new File(""));
        fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
        if (fileChooser.showOpenDialog(frame) == JFileChooser.OPEN_DIALOG) {
            imageFile = fileChooser.getSelectedFile();

            if (Image.getInstance() != null) {
                Image.getInstance().clearImageInstance();
            }

            Image.getInstance(imageFile.getParent(), imageFile.getName());
            frame.dispose();
        } else {
            frame.dispose();
        }
    }
}