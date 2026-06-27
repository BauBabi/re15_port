package baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.selection_object;

import baubabi.reeditor.core.general.utils.UserConfigurationUtility;
import baubabi.reeditor.core.rdt.scd.ScdUtilities;
import baubabi.reeditor.core.rdt.scd._68_op44_sce_em_set.Enemy_sceEmSet_44;
import lombok.Getter;

import javax.swing.*;
import java.awt.*;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

@Getter
public class _44_SceEmSetDisplay {

    //Initialize the Buttons for the bottom
    JPanel panel = new JPanel();

    List<Object> componentList;

    public _44_SceEmSetDisplay(Object currentSelectedScdObject) {
        Enemy_sceEmSet_44 sceEmSet = (Enemy_sceEmSet_44) currentSelectedScdObject;
        componentList = new ArrayList<>();

        componentList.add(sceEmSet);


        JLabel label = new JLabel("Enemy Counter");
        panel.add(label);

        JTextField textField = new JTextField(String.valueOf(sceEmSet.getEnemyCounterDecimal()));
        textField.setName("EnemyCounter");
        textField.addFocusListener(getTextfieldFocusListener());
        panel.add(textField);
        componentList.add(textField);

        label = new JLabel("Enemy Type");
        panel.add(label);

        JComboBox enemyComboBox = fillEnemyComboBox();
        enemyComboBox.setSelectedItem(ScdUtilities.getInstance().getEnemyMapForSelection().get(sceEmSet.getEnemyType()[0]));
        enemyComboBox.setName("EnemyType");
        panel.add(enemyComboBox);
        componentList.add(enemyComboBox);

        label = new JLabel("State");
        panel.add(label);

        if(UserConfigurationUtility.getInstance().isStateAsTextField()) {
            textField = new JTextField(String.valueOf(sceEmSet.getEnemyStateDecimal()));
            textField.setName("State");
            panel.add(textField);
            componentList.add(textField);
        }
        else {
            JComboBox stateComboBox = fillStateComboBox();
            stateComboBox.setSelectedItem(ScdUtilities.getInstance().getStateMapForSelection().get(String.valueOf(sceEmSet.getEnemyStateDecimal())));
            stateComboBox.setName("State");
            panel.add(stateComboBox);
            componentList.add(stateComboBox);
        }

        label = new JLabel("Unknown");
        panel.add(label);

        textField = new JTextField(String.valueOf(sceEmSet.getEnemyUnknownDecimal()));
        textField.setName("Unknown");
        panel.add(textField);
        componentList.add(textField);

        label = new JLabel("Soundbank");
        panel.add(label);

        textField = new JTextField(String.valueOf(sceEmSet.getEnemySoundBankDecimal()));
        textField.setName("Soundbank");
        panel.add(textField);
        componentList.add(textField);

        label = new JLabel("Texture");
        panel.add(label);

        textField = new JTextField(String.valueOf(sceEmSet.getEnemyTextureDecimal()));
        textField.setName("Texture");
        panel.add(textField);
        componentList.add(textField);

        label = new JLabel("Killed ID");
        panel.add(label);

        textField = new JTextField(String.valueOf(sceEmSet.getEnemyKilledIdDecimal()));
        textField.setName("KilledId");
        panel.add(textField);
        componentList.add(textField);

        label = new JLabel("Position X");
        panel.add(label);

        textField = new JTextField(String.valueOf(sceEmSet.getEnemyPositionXDecimal()));
        textField.setName("PositionX");
        panel.add(textField);
        componentList.add(textField);

        label = new JLabel("Position Y");
        panel.add(label);

        textField = new JTextField(String.valueOf(sceEmSet.getEnemyPositionYDecimal()));
        textField.setName("PositionY");
        panel.add(textField);
        componentList.add(textField);

        label = new JLabel("Position Z");
        panel.add(label);

        textField = new JTextField(String.valueOf(sceEmSet.getEnemyPositionZDecimal()));
        textField.setName("PositionZ");
        panel.add(textField);
        componentList.add(textField);

        label = new JLabel("Direction X");
        panel.add(label);

        textField = new JTextField(String.valueOf(sceEmSet.getEnemyDirectionXDecimal()));
        textField.setName("DirectionX");
        panel.add(textField);
        componentList.add(textField);

        label = new JLabel("Direction Y");
        panel.add(label);

        textField = new JTextField(String.valueOf(sceEmSet.getEnemyDirectionYDecimal()));
        textField.setName("DirectionY");
        panel.add(textField);
        componentList.add(textField);

        label = new JLabel("Direction Z");
        panel.add(label);

        textField = new JTextField(String.valueOf(sceEmSet.getEnemyDirectionZDecimal()));
        textField.setName("DirectionZ");
        panel.add(textField);
        componentList.add(textField);

        panel.add(new JSeparator(JSeparator.HORIZONTAL));

        panel.setLayout(new GridLayout(panel.getComponentCount(), 0));
    }

    private JComboBox fillEnemyComboBox() {
        ScdUtilities scdUtilities = ScdUtilities.getInstance();

        Map<String, String> enemyMapForSelection = scdUtilities.getEnemyMapForSelection();
        String[] enemies = new String[enemyMapForSelection.size()];
        int index = 0;

        for (Map.Entry<String, String> entry : enemyMapForSelection.entrySet()) {
            enemies[index] = entry.getValue();
            index = index + 1;
        }

        return new JComboBox(enemies);
    }

    private JComboBox fillStateComboBox() {
        ScdUtilities scdUtilities = ScdUtilities.getInstance();

        Map<String, String> stateMapForSelection = scdUtilities.getStateMapForSelection();
        String[] states = new String[stateMapForSelection.size()];
        int index = 0;

        for (Map.Entry<String, String> entry : stateMapForSelection.entrySet()) {
            states[index] = entry.getValue();
            index = index + 1;
        }

        return new JComboBox(states);
    }

    private FocusListener getTextfieldFocusListener() {
        FocusListener focusListener = new FocusListener()
        {
            @Override
            public void focusLost(FocusEvent arg0)
            {
                JTextField textField = (JTextField) arg0.getSource();
                textField.setText(textField.getText());
                System.out.println("FOCUS LOST");
            }

            @Override
            public void focusGained(FocusEvent arg0)
            {
                // do nothing
            }
        };

        return focusListener;
    }
}
