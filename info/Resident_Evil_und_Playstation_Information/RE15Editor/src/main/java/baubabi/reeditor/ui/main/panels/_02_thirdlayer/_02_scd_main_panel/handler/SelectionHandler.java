package baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler;

import baubabi.reeditor.core.general.utils.ReflectionUtilities;
import baubabi.reeditor.core.rdt.scd._68_op44_sce_em_set.Enemy_sceEmSet_44;
import baubabi.reeditor.ui.main.panels._02_thirdlayer.WipPage;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.selection_object._44_SceEmSetDisplay;
import lombok.Getter;
import lombok.Setter;

import javax.swing.*;
import java.awt.*;

@Getter
@Setter
public class SelectionHandler {
    Object currentSelectedScdObject;

    Object currentCorrespondingScdObjectClass;

    public SelectionHandler(Object selectedScdElement) {
        this.currentSelectedScdObject = selectedScdElement;
    }

    public JPanel getNewSelectionPanel() {
        //Fetch Scd Panel for it or the WIP page if not implemented yet
        currentCorrespondingScdObjectClass = fetchPanelForScdObject();

        return buildUpUi();
    }

    private Object fetchPanelForScdObject() {

        //Here we specify the specific panels for scd data types
        if(currentSelectedScdObject instanceof Enemy_sceEmSet_44) {
            _44_SceEmSetDisplay sceEmSetDisplay = new _44_SceEmSetDisplay(currentSelectedScdObject);
            return sceEmSetDisplay;
        }
        else {
            WipPage wipPage = new WipPage(20);
            return wipPage;
        }
    }

    public JPanel buildUpUi() {
        //Define new elementScd Element panel
        JPanel finalElementPanel = new JPanel(false);
        finalElementPanel.setLayout(new GridBagLayout());
        GridBagConstraints c = new GridBagConstraints();

        JPanel elementPanel = new JPanel();

        c.fill = GridBagConstraints.BOTH;
        c.weightx = 0.66;
        c.weighty = 1.0;
        c.anchor = GridBagConstraints.NORTH;

        if(currentCorrespondingScdObjectClass instanceof WipPage) {
            //Fetch Panel for WIP Page, when data is not implemented yet
            WipPage wipPage = (WipPage) currentCorrespondingScdObjectClass;
            elementPanel.setLayout(new BorderLayout());
            elementPanel.add(wipPage.getPanel());
        }
        else {
            elementPanel = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnJPanel(currentCorrespondingScdObjectClass, "getPanel", this.getClass().getCanonicalName());
        }

        elementPanel.setLayout(new GridLayout(elementPanel.getComponentCount(), 0));

        JScrollPane scrollPane = new JScrollPane(elementPanel);
        finalElementPanel.add(scrollPane, c);

        return finalElementPanel;
    }
}
