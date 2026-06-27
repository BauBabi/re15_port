package baubabi.reeditor.ui.main.panels._02_thirdlayer;

import baubabi.reeditor.ui.abstracts.AbstractWindowText;
import lombok.extern.log4j.Log4j2;

import javax.swing.*;
import java.util.ArrayList;
import java.util.List;

@Log4j2
public class WipPage extends AbstractWindowText {
    List<Object> componentList = new ArrayList<>();

    public WipPage() {
        super(40);
        ArrayList<String> textForTextPane = new ArrayList<>();
        textForTextPane.add("This feature has not yet been implemented");
        textForTextPane.add("");
        textForTextPane.add("Please try it again in a later version.");
        addTextToTextPanel(textForTextPane);
    }

    public WipPage(int fontSize) {
        super(fontSize);

        ArrayList<String> textForTextPane = new ArrayList<>();
        textForTextPane.add("This feature has not yet been implemented");
        textForTextPane.add("");
        textForTextPane.add("Please try it again in a later version.");
        addTextToTextPanel(textForTextPane);
    }

    @Override
    public JPanel getPanel() {
        return panel;
    }

    public List<Object> getComponentList() {
        return componentList;
    }
}
