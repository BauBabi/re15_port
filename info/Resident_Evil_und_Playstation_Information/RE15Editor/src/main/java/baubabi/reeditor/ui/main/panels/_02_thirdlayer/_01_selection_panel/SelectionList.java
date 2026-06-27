package baubabi.reeditor.ui.main.panels._02_thirdlayer._01_selection_panel;

import baubabi.reeditor.core.exception.ExceptionHandler;
import baubabi.reeditor.core.general.utils.ReflectionUtilities;
import baubabi.reeditor.core.rdt._40_main_scd.MainScd40;
import baubabi.reeditor.core.rdt._42_sub_scd.SubScd42;
import baubabi.reeditor.core.rdt._44_extra_scd.ExtraScd44;
import baubabi.reeditor.core.rdt.scd.ScdUtilities;
import baubabi.reeditor.ui.components.ObjectListSelectionNameHandler;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.ListTransferHandler;
import lombok.Getter;

import javax.swing.*;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import java.awt.*;
import java.awt.event.*;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.List;
import java.util.Map;

@Getter
public class SelectionList {

    private final DefaultListModel model = new DefaultListModel();

    private final JList list = new JList(model);

    private final JPanel selectionPanel = new JPanel(false);

    private final JButton addButton = new JButton("Add Element");

    private final JButton removeButton = new JButton("Remove Element");

    JPopupMenu popup = new JPopupMenu();

    DefaultListModel modelElement = new DefaultListModel();

    private static final int CHAR_DELTA = 1000;
    private String m_key;
    private long m_time;

    JList scdElementlist = new JList(modelElement);

    JScrollPane scrollPaneScd;

    private final PropertyChangeSupport changes = new PropertyChangeSupport(this);

    private final JTabbedPane tabbedPaneFromParent;

    private final Map<String, Object> scdMap;

    public SelectionList(Object scdElementObject, JTabbedPane tabbedPaneFromParent, Map<String, Object> scdMap) {
        this.tabbedPaneFromParent = tabbedPaneFromParent;
        this.scdMap = scdMap;

        //Initialize selection Panel
        selectionPanel.setLayout(new BorderLayout());
        selectionPanel.setName("SelectionPanel");

        fetchElementsForSelection(scdElementObject);

        //Elements for Add Button
        List<Class> scdElementList = ScdUtilities.getInstance().getScdElementList();

        for (int i = 0; i < scdElementList.size(); i++) {
            try {
                Class<?> clazz = Class.forName(scdElementList.get(i).getCanonicalName());
                Constructor<?> ctor = clazz.getConstructor();
                Object object = ctor.newInstance();
                modelElement.addElement(object);
            } catch (ClassNotFoundException e) {
                ExceptionHandler.getInstance().logException("ClassNotFoundException at getting instance of AddElements in SelectorList", e);
            } catch (NoSuchMethodException e) {
                ExceptionHandler.getInstance().logException("NoSuchMethodException at getting instance of AddElements in SelectorList", e);
            } catch (InvocationTargetException e) {
                ExceptionHandler.getInstance().logException("InvocationTargetException at getting instance of AddElements in SelectorList", e);
            } catch (InstantiationException e) {
                ExceptionHandler.getInstance().logException("InstantiationException at getting instance of AddElements in SelectorList", e);
            } catch (IllegalAccessException e) {
                ExceptionHandler.getInstance().logException("IllegalAccessException at getting instance of AddElements in SelectorList", e);
            }
        }

        initializeListeners();

        //For drag and Drop
        list.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
        list.setDragEnabled(true);
        list.setTransferHandler(new ListTransferHandler(tabbedPaneFromParent ,scdMap));
        list.setDropMode(DropMode.INSERT);

        //AREA FOR ADD BUTTON
        //Give the List to the Scroll Pane
        JScrollPane scrollPane = new JScrollPane(list);

        //Give the Scroll Pane to the selectionPanel
        selectionPanel.add(scrollPane, BorderLayout.CENTER);


        //Get a human readable name from the scd elements (the "displayName" element).
        scdElementlist.setCellRenderer(new ObjectListSelectionNameHandler());

        scrollPaneScd = new JScrollPane(scdElementlist);
        scrollPaneScd.setPreferredSize(new Dimension(200, 300));

        popup.add(scrollPaneScd);

        addButton.setComponentPopupMenu(popup);

        //Initialize the Buttons for the bottom
        JPanel buttonPane = new JPanel();
        buttonPane.add(addButton);
        buttonPane.add(removeButton);

        //Add the Buttons to the bottom of the selection Panel
        selectionPanel.add(buttonPane, BorderLayout.SOUTH);

        selectionPanel.setVisible(true);
    }

    private void initializeListeners() {
        MouseListener mouseListener = new MouseAdapter() {
            public void mousePressed(MouseEvent e) { }
            public void mouseClicked(MouseEvent e) { checkPopup(e); }
            public void mouseReleased(MouseEvent e) { }
            private void checkPopup(MouseEvent e) {
                popup.show(e.getComponent(), (addButton.getX() - addButton.getWidth()), (-(addButton.getY() + (int) addButton.getAlignmentY() + (int) popup.getPreferredSize().getHeight())));
            }
        };

        addButton.addMouseListener(mouseListener);

        scdElementlist.addKeyListener(new KeyListener() {
            @Override
            public void keyTyped(KeyEvent e) {}
            @Override
            public void keyPressed(KeyEvent e) {
                char ch = e.getKeyChar();
                System.out.println(ch);

                if(e.getKeyCode() == 10) {
                    if(scdElementlist.getSelectedValue() != null) {
                        //Enter Pressed
                        Object newInstanceOfSelectedObject = null;
                        try {
                            Constructor<?> declaredConstructorOfElement = scdElementlist.getSelectedValue().getClass().getDeclaredConstructor();
                            newInstanceOfSelectedObject = declaredConstructorOfElement.newInstance();
                        } catch (NoSuchMethodException ex) {
                            ExceptionHandler.getInstance().logException("NoSuchMethodException at getting constructor of new element to add", ex);
                        } catch (InvocationTargetException ex) {
                            ExceptionHandler.getInstance().logException("InvocationTargetException at getting constructor of new element to add", ex);
                        } catch (InstantiationException ex) {
                            ExceptionHandler.getInstance().logException("InstantiationException at getting constructor of new element to add", ex);
                        } catch (IllegalAccessException ex) {
                            ExceptionHandler.getInstance().logException("IllegalAccessException at getting constructor of new element to add", ex);
                        }
                        changes.firePropertyChange("addPressed", null, newInstanceOfSelectedObject);

                        //Add new Element to the Selection List
                        model.addElement(newInstanceOfSelectedObject);

                        ListTransferHandler listTransferHandler = (ListTransferHandler) list.getTransferHandler();

                        listTransferHandler.setAddCount(listTransferHandler.getAddCount() + 1);

                        //Remove Popup Window
                        popup.setVisible(false);
                    }
                }

                //ignore searches for non alpha-numeric characters other than ENTER
                else if (!Character.isLetterOrDigit(ch)) {}
                else {
                    // Check pressed key and search in list
                    if (m_time+CHAR_DELTA < System.currentTimeMillis()) {
                        m_key = "";
                    }

                    m_time = System.currentTimeMillis();
                    m_key += Character.toLowerCase(ch);

                    for (int i=0; i < scdElementlist.getModel().getSize(); i++) {
                        Object currentObjectToSearch = scdElementlist.getModel().getElementAt(i);
                        String displayNameOfObject = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnString(currentObjectToSearch, "getDisplayName", SelectionList.class.getCanonicalName());

                        if (displayNameOfObject.startsWith(m_key)) {
                            scdElementlist.setSelectedIndex(i);     //change selected item in list
                            scdElementlist.ensureIndexIsVisible(i); //change listbox scroll-position
                            break;
                        }
                    }
                }
            }

            @Override
            public void keyReleased(KeyEvent e) {}
        });

        removeButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                removeElement();
            }
        });
        list.addListSelectionListener(new ListSelectionListener() {
            @Override
            public void valueChanged(ListSelectionEvent e) {
                //And now adjust the second Window with the values of the object
                if(list.getSelectedValue() != null) {
                    changes.firePropertyChange("elementSelected", null, list.getSelectedValue());
                }
            }
        });
        list.addKeyListener(new KeyAdapter() {
            @Override
            public void keyTyped(KeyEvent e) {
                super.keyTyped(e);
                char ch = e.getKeyChar();
                if(ch == KeyEvent.VK_DELETE) {
                    removeElement();
                }
            }
        });
    }

    private void removeElement() {
        if (model.getSize() > 0) {
            changes.firePropertyChange("removePressed", null, list.getSelectedValue());
            Object selectedObject = list.getSelectedValue();
            list.clearSelection();
            model.removeElement(selectedObject);
        }
    }

    private void fetchElementsForSelection(Object scdElementObject) {
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
        if(objectList != null) {
            for(int i = 0; i < objectList.size(); i++) {
                model.addElement(objectList.get(i));

                //Get a human readable name from the scd elements (the "displayName" element).
                list.setCellRenderer(new ObjectListSelectionNameHandler());
            }
        }
    }

    public void addPropertyChangeListener(PropertyChangeListener l) {
        changes.addPropertyChangeListener(l);
    }

    public void removePropertyChangeListener(PropertyChangeListener l) {
        changes.removePropertyChangeListener(l);
    }
}
