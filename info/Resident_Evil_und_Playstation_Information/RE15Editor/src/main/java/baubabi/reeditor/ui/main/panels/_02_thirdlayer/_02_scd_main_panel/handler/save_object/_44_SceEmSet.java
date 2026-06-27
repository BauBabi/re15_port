package baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.save_object;

import baubabi.reeditor.core.rdt.scd.ScdUtilities;
import baubabi.reeditor.core.rdt.scd._68_op44_sce_em_set.Enemy_sceEmSet_44;

import javax.swing.*;
import java.util.List;

public class _44_SceEmSet {

    public void saveEnemyData(Enemy_sceEmSet_44 enemy, List<Object> componentList) {
        for(int i = 1; i < componentList.size(); i++) {
            JTextField textField;
            JComboBox comboBox;

            Object component = componentList.get(i);

            if (component instanceof JTextField) {
                textField = (JTextField) component;
                if (textField.getName().equals("EnemyCounter")) {
                    enemy.setEnemyCounter(textField.getText());
                } else if (textField.getName().equals("State")) {
                    enemy.setEnemyState(textField.getText());
                } else if (textField.getName().equals("Unknown")) {
                    enemy.setEnemyUnknown(textField.getText());
                } else if (textField.getName().equals("Soundbank")) {
                    enemy.setEnemySoundBank(textField.getText());
                } else if (textField.getName().equals("Texture")) {
                    enemy.setEnemyTexture(textField.getText());
                } else if (textField.getName().equals("KilledId")) {
                    enemy.setEnemyKilledId(textField.getText());
                } else if (textField.getName().equals("PositionX")) {
                    enemy.setEnemyPositionX(textField.getText());
                } else if (textField.getName().equals("PositionY")) {
                    enemy.setEnemyPositionY(textField.getText());
                } else if (textField.getName().equals("PositionZ")) {
                    enemy.setEnemyPositionZ(textField.getText());
                } else if (textField.getName().equals("DirectionX")) {
                    enemy.setEnemyDirectionX(textField.getText());
                } else if (textField.getName().equals("DirectionY")) {
                    enemy.setEnemyDirectionY(textField.getText());
                } else if (textField.getName().equals("DirectionZ")) {
                    enemy.setEnemyDirectionZ(textField.getText());
                }
            } else if (component instanceof JComboBox) {
                comboBox = (JComboBox) component;
                if (comboBox.getName().equals("EnemyType")) {
                    enemy.setEnemyType(ScdUtilities.getInstance().getEnemyMapForSaving().get(comboBox.getSelectedItem().toString()));
                }
                else if (comboBox.getName().equals("State")) {
                    enemy.setEnemyState(ScdUtilities.getInstance().getStateMapForSaving().get(comboBox.getSelectedItem().toString()));
                }
            }
        }
    }
}
