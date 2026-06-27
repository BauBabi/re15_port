package baubabi.reeditor.ui.main.panels._02_thirdlayer;

import baubabi.reeditor.core.rdt.RdtProcessing;
import baubabi.reeditor.core.rdt._40_main_scd.MainScd40;
import baubabi.reeditor.core.rdt._42_sub_scd.SubScd42;
import baubabi.reeditor.core.rdt._44_extra_scd.ExtraScd44;
import baubabi.reeditor.core.rdt.scd.ScdUtilities;
import baubabi.reeditor.ui.Utilities.InteractionCommunicationHelperUtility;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._01_selection_panel.SelectionList;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.ScdMainPanel;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.SaveHandler;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.common.AddRemoveHandlerCommons;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._03_image_panel.ImagePanel;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._04_log_panel.LogPanel;
import lombok.Getter;
import lombok.extern.log4j.Log4j2;

import javax.swing.*;
import java.awt.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Getter
@Log4j2
public class InfoDetailTab implements PropertyChangeListener {

    private final JTabbedPane tabbedPane = new JTabbedPane();

    private final RdtProcessing rdtProcessing;

    private final Map<String, Object> scdMap = new HashMap<>();

    ScdUtilities scdUtilities;

    InteractionCommunicationHelperUtility interactionCommunicationHelperUtility;

    JPanel panel;

    //Our 4 Panels
    SelectionList selectionList;
    ScdMainPanel scdMainPanel;
    ImagePanel imagePanel;
    LogPanel logPanel;

    HashMap<String, Object> panelHolder = new HashMap<>();


    public InfoDetailTab() {

        interactionCommunicationHelperUtility = InteractionCommunicationHelperUtility.getInstance();
        interactionCommunicationHelperUtility.setInfoDetailTabPropertyChangeListener(this);
        rdtProcessing = RdtProcessing.getInstance();
        scdUtilities = ScdUtilities.getInstance();

        List<String> rdtTabs = fetchRdtTabTexts();

        for (int i = 0; i < rdtTabs.size(); i++) {
            JComponent panelScd = initializeScdTab(rdtTabs.get(i));
            tabbedPane.addTab(rdtTabs.get(i), null, panelScd, "Element Panel");
        }

        tabbedPane.setVisible(true);
    }

    //Fetches rdtTab Texts from the scds
    private List<String> fetchRdtTabTexts() {
        List<String> rdtList = new ArrayList<>();

        List<Object> rdtObjectList = rdtProcessing.getAllScdDataElements();

        int mainCounter = 0;

        int subCounter = 0;

        int extraCounter = 0;

        for (int i = 0; i < rdtObjectList.size(); i++) {
            if (rdtObjectList.get(i) instanceof MainScd40) {
                String tabName = "main" + String.format("%02d", mainCounter) + ".scd";
                rdtList.add(tabName);
                scdMap.put(tabName, rdtObjectList.get(i));
                mainCounter = mainCounter + 1;
            } else if (rdtObjectList.get(i) instanceof SubScd42) {
                String tabName = "sub" + String.format("%02d", subCounter) + ".scd";
                rdtList.add(tabName);
                scdMap.put(tabName, rdtObjectList.get(i));
                subCounter = subCounter + 1;
            } else if (rdtObjectList.get(i) instanceof ExtraScd44) {
                String tabName = "extra" + String.format("%02d", extraCounter) + ".scd";
                rdtList.add(tabName);
                scdMap.put(tabName, rdtObjectList.get(i));
                extraCounter = extraCounter + 1;
            }
        }

        return rdtList;
    }

    protected JComponent initializeScdTab(String text) {

        //Sub Panel for the Tab
        panel = new JPanel(false);
        panel.setLayout(new GridLayout(0, 4));
        panel.setBorder(BorderFactory.createEmptyBorder(2, 2, 2, 2));
        panel.setName(text);

        //1. Adds the selection list on the left for the SCD elements.
        selectionList = new SelectionList(scdMap.get(text), tabbedPane, scdMap);
        selectionList.addPropertyChangeListener(this);
        panel.add(selectionList.getSelectionPanel());

        //2. Adds the second Panel, to configure the selected SCD element main informations like Position X, Y, Z etc.
        scdMainPanel = new ScdMainPanel(text);
        panel.add(scdMainPanel.getScdMainPanel());

        //3. The third Panel - showing the Image. Here we have the enemy picture on the top side, and the Room picture on the lower site.
        imagePanel = new ImagePanel();
        panel.add(imagePanel.getImagePanel());

        //4. The fourth and last Panel for the logs.
        logPanel = new LogPanel();
        panel.add(logPanel.getLogPanel());

        panelHolder.put(text + "_selectionList", selectionList);
        panelHolder.put(text + "_scdMainPanel", scdMainPanel);
        panelHolder.put(text + "_imagePanel", imagePanel);
        panelHolder.put(text + "_logPanel", logPanel);

        return panel;
    }

    public void propertyChange(PropertyChangeEvent evt) {
        if(evt.getPropertyName().equals("elementSelected")) {
            JPanel currentScdTab = (JPanel) tabbedPane.getSelectedComponent();
            currentScdTab.remove(1);

            imagePanel.prepareEnemyImage(evt.getNewValue());
            scdMainPanel.updateScdMainPanelForCurrentScdSelection(evt.getNewValue());

            currentScdTab.add(scdMainPanel.getScdMainPanel(), 1);

            tabbedPane.repaint();
            tabbedPane.setVisible(true);
        }
        else if(evt.getPropertyName().equals("savePressed")) {
            writeLog("Saving... ");
            if(scdMainPanel.getSelectionHandler() != null) {
                scdMainPanel.getOldValues().put(scdMainPanel.getSelectionHandler().getCurrentSelectedScdObject(), scdMainPanel.getSelectionHandler().getCurrentCorrespondingScdObjectClass());
                new SaveHandler(scdMainPanel, scdMap);

                writeLog("Writing modified Image done");
                scdMainPanel.setOldValues(new HashMap<>());
            }
        }
        else if(evt.getPropertyName().equals("removePressed")) {
            int currentlySelectedIndex = tabbedPane.getSelectedIndex();
            String nameOfSelectedTab = tabbedPane.getTitleAt(currentlySelectedIndex);
            Object scdElementTab = this.scdMap.get(nameOfSelectedTab);

            AddRemoveHandlerCommons addRemoveHandlerCommons = new AddRemoveHandlerCommons();
            addRemoveHandlerCommons.modifyElement(evt.getNewValue(), scdElementTab, true);
            selectionList.getList().clearSelection();
            selectionList.getList().setSelectedIndex(0);
            scdMainPanel.getOldValues().remove(evt.getNewValue());
        }
        else if(evt.getPropertyName().equals("addPressed")) {
            int currentlySelectedIndex = tabbedPane.getSelectedIndex();
            String nameOfSelectedTab = tabbedPane.getTitleAt(currentlySelectedIndex);
            Object scdElementTab = this.scdMap.get(nameOfSelectedTab);
            AddRemoveHandlerCommons addRemoveHandlerCommons = new AddRemoveHandlerCommons();
            addRemoveHandlerCommons.modifyElement(evt.getNewValue(), scdElementTab, false);
        }
    }

    public void writeLog(String log) {
        for(int i = 0; i < tabbedPane.getComponentCount(); i++) {
            LogPanel logPanelCurrent = (LogPanel) panelHolder.get(tabbedPane.getComponent(i).getName() + "_logPanel");
            logPanelCurrent.writeLog(log);
        }

        tabbedPane.repaint();
        tabbedPane.setVisible(true);
    }
}
