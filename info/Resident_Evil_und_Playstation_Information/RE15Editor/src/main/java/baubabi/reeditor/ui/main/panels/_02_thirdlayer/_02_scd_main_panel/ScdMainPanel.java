package baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel;

import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.SelectionHandler;
import lombok.Getter;

import javax.swing.*;
import java.awt.*;
import java.util.HashMap;

@Getter
public class ScdMainPanel {
    private JPanel scdMainPanel = new JPanel(false);

    private SelectionHandler selectionHandler;

    private HashMap<Object, Object> oldValues;

    public ScdMainPanel(String text) {
        JLabel startRdtText = new JLabel(text);
        startRdtText.setHorizontalAlignment(JLabel.CENTER);
        scdMainPanel.setBorder(BorderFactory.createMatteBorder(1, 1, 1, 0, Color.gray));
        scdMainPanel.setLayout( new GridBagLayout() );
        scdMainPanel.setName("ScdMainPanel");
        scdMainPanel.add(startRdtText, new GridBagConstraints());
    }

    public void updateScdMainPanelForCurrentScdSelection(Object currentScdSelection) {
        if(oldValues == null) {
            oldValues = new HashMap<>();
        }

        Object selectedObjectFromMap = oldValues.get(currentScdSelection);
        selectionHandler = new SelectionHandler(currentScdSelection);
        if(selectedObjectFromMap == null) {
            scdMainPanel = selectionHandler.getNewSelectionPanel();
            oldValues.put(currentScdSelection, selectionHandler.getCurrentCorrespondingScdObjectClass());
        }
        else {
            selectionHandler.setCurrentCorrespondingScdObjectClass(selectedObjectFromMap);
            scdMainPanel = selectionHandler.buildUpUi();
        }
    }

    public void setOldValues(HashMap<Object, Object> oldValues) {
        this.oldValues = oldValues;
    }
}
