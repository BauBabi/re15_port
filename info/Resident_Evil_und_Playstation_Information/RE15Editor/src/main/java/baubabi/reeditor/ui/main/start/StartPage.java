package baubabi.reeditor.ui.main.start;

import baubabi.reeditor.ui.abstracts.AbstractWindowText;
import lombok.extern.log4j.Log4j2;

import javax.swing.*;
import java.util.ArrayList;

@Log4j2
public class StartPage extends AbstractWindowText {
    public StartPage() {
        super(40);

        ArrayList<String> textForTextPane = new ArrayList<>();
        textForTextPane.add("Welcome to the RE 1.5 Editor");
        textForTextPane.add("");
        textForTextPane.add("Please start by loading your Resident Evil 1.5 Image in .bin format in the File Menu.");

        addTextToTextPanel(textForTextPane);
    }


    @Override
    public JPanel getPanel() {
        return panel;
    }
}
