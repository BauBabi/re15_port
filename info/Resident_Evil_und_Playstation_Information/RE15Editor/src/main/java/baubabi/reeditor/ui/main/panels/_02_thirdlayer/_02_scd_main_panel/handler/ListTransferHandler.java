package baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler;

import baubabi.reeditor.core.general.utils.ReflectionUtilities;
import baubabi.reeditor.core.rdt._40_main_scd.MainScd40;
import baubabi.reeditor.core.rdt._42_sub_scd.SubScd42;
import baubabi.reeditor.core.rdt._44_extra_scd.ExtraScd44;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.utilities.HandlerUtilities;

import javax.swing.*;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import java.util.List;
import java.util.Map;

public class ListTransferHandler extends TransferHandler {
    private int[] indices = null;
    private int addIndex = -1; //Location where items were added
    private int addCount = 0;  //Number of items added.

    JTabbedPane tabbedPaneFromParent;

    Map<String, Object> scdMap;

    public ListTransferHandler( JTabbedPane tabbedPaneFromParent, Map<String, Object> scdMap) {
        this.scdMap = scdMap;
        this.tabbedPaneFromParent = tabbedPaneFromParent;
    }

    public boolean canImport(TransferSupport info) {
        // Check for String flavor
        return info.isDataFlavorSupported(DataFlavor.stringFlavor);
    }

    protected Transferable createTransferable(JComponent c) {
        return new StringSelection(exportString(c));
    }

    public int getSourceActions(JComponent c) {
        return TransferHandler.COPY_OR_MOVE;
    }

    public boolean importData(TransferSupport info) {
        if (!info.isDrop()) {
            return false;
        }

        JList list = (JList)info.getComponent();
        DefaultListModel listModel = (DefaultListModel)list.getModel();
        JList.DropLocation dl = (JList.DropLocation)info.getDropLocation();
        int index = dl.getIndex();
        boolean insert = dl.isInsert();

        Object data = list.getSelectedValue();

        // Perform the actual import.
        if (insert) {
            listModel.add(index, data);
        } else {
            listModel.set(index, data);
        }

        //Update RDT List
        int currentlySelectedIndex = tabbedPaneFromParent.getSelectedIndex();
        String nameOfSelectedTab = tabbedPaneFromParent.getTitleAt(currentlySelectedIndex);
        Object scdElementTab = this.scdMap.get(nameOfSelectedTab);

        List<Object> singleScdDataElements = fetchObjectListForRdtTab(scdElementTab, listModel, list);
        int indexOfObjectToDelete = singleScdDataElements.indexOf(data);
        singleScdDataElements.add(index, data);
        singleScdDataElements.remove(indexOfObjectToDelete + 1);

        //Update the ScdData
        String newMainScdData = HandlerUtilities.getInstance().fetchAndReplaceScdElementData(singleScdDataElements);
        ReflectionUtilities.getInstance().executeMethodForClassObjectWithStringParameter(scdElementTab, "setScdData", this.getClass().getCanonicalName(), newMainScdData);

        return true;
    }

    protected void exportDone(JComponent c, Transferable data, int action) {
        cleanup(c, action == TransferHandler.MOVE);
    }

    //Bundle up the selected items in the list
    //as a single string, for export.
    protected String exportString(JComponent c) {
        JList list = (JList)c;
        indices = list.getSelectedIndices();
        Object[] values = list.getSelectedValues();

        StringBuffer buff = new StringBuffer();

        for (int i = 0; i < values.length; i++) {
            Object val = values[i];
            buff.append(val == null ? "" : val.toString());
            if (i != values.length - 1) {
                buff.append("\n");
            }
        }

        return buff.toString();
    }

    //Take the incoming string and wherever there is a
    //newline, break it into a separate item in the list.
    protected void importString(JComponent c, String str) {
        JList target = (JList)c;
        DefaultListModel listModel = (DefaultListModel)target.getModel();
        int index = target.getSelectedIndex();

        //Prevent the user from dropping data back on itself.
        //For example, if the user is moving items #4,#5,#6 and #7 and
        //attempts to insert the items after item #5, this would
        //be problematic when removing the original items.
        //So this is not allowed.
        if (indices != null && index >= indices[0] - 1 &&
                index <= indices[indices.length - 1]) {
            indices = null;
            return;
        }

        int max = listModel.getSize();
        if (index < 0) {
            index = max;
        } else {
            index++;
            if (index > max) {
                index = max;
            }
        }
        addIndex = index;
        String[] values = str.split("\n");
        addCount = values.length;
        for (int i = 0; i < values.length; i++) {
            listModel.add(index++, values[i]);
        }
    }

    //If the remove argument is true, the drop has been
    //successful and it's time to remove the selected items
    //from the list. If the remove argument is false, it
    //was a Copy operation and the original list is left
    //intact.
    protected void cleanup(JComponent c, boolean remove) {
        if (remove && indices != null) {
            JList source = (JList)c;
            DefaultListModel model  = (DefaultListModel)source.getModel();
            //If we are moving items around in the same list, we
            //need to adjust the indices accordingly, since those
            //after the insertion point have moved.
            if (addCount > 0) {
                for (int i = 0; i < indices.length; i++) {
                    if (indices[i] > addIndex) {
                        indices[i] += addCount;
                    }
                }
            }
            for (int i = indices.length - 1; i >= 0; i--) {
                model.remove(indices[i]);
            }
        }
        indices = null;
        addCount = 0;
        addIndex = -1;
    }

    private List<Object> fetchObjectListForRdtTab(Object scdElementObject, DefaultListModel model, JList list) {
        List<Object> objectList = null;
        if(scdElementObject instanceof MainScd40) {
            MainScd40 mainScd40 = (MainScd40) scdElementObject;
            objectList = mainScd40.getSingleScdDataElements();
        }
        else if(scdElementObject instanceof SubScd42) {
            SubScd42 subScd42 = (SubScd42) scdElementObject;
            objectList = subScd42.getSingleScdDataElements();
        }
        else if(scdElementObject instanceof ExtraScd44) {
            ExtraScd44 extraScd44 = (ExtraScd44) scdElementObject;
            objectList = extraScd44.getSingleScdDataElements();
        }

        return objectList;
    }

    public int getAddCount() {
        return addCount;
    }

    public void setAddCount(int addCount) {
        this.addCount = addCount;
    }
}
