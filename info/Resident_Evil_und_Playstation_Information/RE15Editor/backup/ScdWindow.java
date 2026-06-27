package baubabi.reeditor.ui.components.backup;

import baubabi.reeditor.core.image.utils.ImageUtilities;
import lombok.Getter;

import javax.swing.*;
import java.util.List;
import java.util.Map;

import static javax.swing.WindowConstants.DISPOSE_ON_CLOSE;

@Getter
public class ScdWindow {

    JFrame frame = new JFrame ("Test");

    public ScdWindow(Map<String, List<Object>> finalListOfScdElements, String pathToRDT, ImageUtilities imageUtilities) {
        frame.setSize(1800, 1000);

        ScdWindowElement scdWindowElement = new ScdWindowElement(finalListOfScdElements, pathToRDT, imageUtilities);
        frame.getContentPane().add(scdWindowElement.getElementPanel());

        frame.setVisible(true);

        frame.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
    }
}
