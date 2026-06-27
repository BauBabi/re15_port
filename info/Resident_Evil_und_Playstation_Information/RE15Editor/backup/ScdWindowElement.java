package baubabi.reeditor.ui.components.backup;

import baubabi.reeditor.core.general.utils.ProcessUtilities;
import baubabi.reeditor.core.image.utils.ImageUtilities;
import baubabi.reeditor.core.rdt.RdtProcessing;
import baubabi.reeditor.core.rdt.scd._68_op44_sce_em_set.Enemy_sceEmSet_44;
import lombok.Getter;
import lombok.extern.log4j.Log4j2;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Getter
@Log4j2
public class ScdWindowElement {
    JPanel elementPanel = new JPanel(false);

    List<JPanel> elementFields = new ArrayList<>();

    Map<String, String> enemyMap = new HashMap<>();

    List<Object> componentList;

    Map<String, List<Object>> componentMap = new HashMap<>();

    JPanel elementPane;

    Timer timer;

    public ScdWindowElement(Map<String, List<Object>> finalListOfScdElements, String pathToRDT, ImageUtilities imageUtilities) {
        enemyMap.put("10", "POLICE ZOMBIE SLIM - 10");
        enemyMap.put("11", "POLICE ZOMBIE FAT - 11");
        enemyMap.put("16", "CIVIL ZOMBIE - 16");
        enemyMap.put("20", "DOG - 20");
        enemyMap.put("21", "CROW - 21");
        enemyMap.put("25", "SPIDER - 25");
        enemyMap.put("27", "MONKEY - 27");
        enemyMap.put("47", "BOX? - 47");

        fetchElementsByClass(finalListOfScdElements);

        elementPanel.setLayout(new GridBagLayout());
        GridBagConstraints c = new GridBagConstraints();

        elementPane = new JPanel();
        c.fill = GridBagConstraints.BOTH;
        c.weightx = 0.66;
        c.weighty = 1.0;
        c.anchor = GridBagConstraints.NORTH;

        JButton button = new JButton("save");

        for(int i = 0; i < elementFields.size(); i++) {
            elementPane.add(elementFields.get(i));
        }
        elementPane.add(button);



        elementPane.setLayout(new GridLayout(elementPane.getComponentCount(), 0));


        JScrollPane scrollPane = new JScrollPane(elementPane);
        elementPanel.add(scrollPane, c);


        elementPane = new JPanel();
        c.fill = GridBagConstraints.HORIZONTAL;
        c.weightx = 0.33;
        c.weighty = 1.0;

        JLabel text;
        text = new JLabel("Text 2");
        elementPane.add(text);
        elementPanel.add(elementPane, c);

        button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                System.out.println("Saving....");
                ProcessUtilities processUtilities = ProcessUtilities.getInstance();
                List<String> scdDataNew = RdtProcessing.getInstance().getScdData();
                StringBuilder emDataOld = new StringBuilder();
                StringBuilder emDataNew = new StringBuilder();

                for ( String key : componentMap.keySet() ) {
                    String mapKey = key;
                    String opcode = key.split("-")[0];
                    emDataNew.append(opcode);
                    List<Object> mapObject = componentMap.get(mapKey);
                    for(int i = 0; i < mapObject.size(); i++) {
                        if(mapObject.get(i) instanceof JTextField) {
                            JTextField textField = (JTextField) mapObject.get(i);
                            System.out.println(textField.getName());
                            System.out.println(textField.getText());

                            if(textField.getName().contains("Direction") || textField.getName().contains("Position")) {
                                if(Integer.parseInt(textField.getText()) < 0) {
                                    String hex = String.format("%02X", Integer.parseInt(textField.getText()));
                                    hex = hex.substring(4,hex.length());
                                    emDataNew.append(hex);
                                }
                                else {
                                    String hex = String.format("%02X", Integer.parseInt(textField.getText()));
                                    if(hex.length() < 4) {
                                        emDataNew.append("00");
                                    }
                                    emDataNew.append(hex);
                                }
                            }
                            else {
                                if(Integer.parseInt(textField.getText()) < 0) {
                                    String hex = String.format("%02X", Integer.parseInt(textField.getText()));
                                    hex = hex.substring(4,hex.length());
                                    emDataNew.append(hex);
                                }
                                else {
                                    emDataNew.append(String.format("%02X", Integer.parseInt(textField.getText())));
                                }
                            }
                        }
                        else if(mapObject.get(i) instanceof JComboBox) {
                            JComboBox comboBox = (JComboBox) mapObject.get(i);
                            System.out.println(comboBox.getName());
                            System.out.println(comboBox.getSelectedItem());
                            String enemy = (String) comboBox.getSelectedItem();
                            enemy = enemy.split("-")[1];
                            enemy = enemy.replaceAll(" ", "");
                            emDataNew.append(enemy);
                        }
                        else if(mapObject.get(i) instanceof Enemy_sceEmSet_44) {
                            Enemy_sceEmSet_44 sceEmSet = (Enemy_sceEmSet_44) mapObject.get(i);
                            emDataOld.append(sceEmSet.getOPCODE());
                            emDataOld.append(sceEmSet.getEnemyCounter()[0]);
                            emDataOld.append(sceEmSet.getEnemyType()[0]);
                            emDataOld.append(sceEmSet.getEnemyState()[0]);
                            emDataOld.append(sceEmSet.getEnemyUnknown()[0]);
                            emDataOld.append(sceEmSet.getEnemySoundBank()[0]);
                            emDataOld.append(sceEmSet.getEnemyTexture()[0]);
                            emDataOld.append(sceEmSet.getEnemyKilledId()[0]);
                            emDataOld.append(sceEmSet.getEnemyPositionX()[0]);
                            emDataOld.append(sceEmSet.getEnemyPositionX()[1]);
                            emDataOld.append(sceEmSet.getEnemyPositionY()[0]);
                            emDataOld.append(sceEmSet.getEnemyPositionY()[1]);
                            emDataOld.append(sceEmSet.getEnemyPositionZ()[0]);
                            emDataOld.append(sceEmSet.getEnemyPositionZ()[1]);
                            emDataOld.append(sceEmSet.getEnemyDirectionX()[0]);
                            emDataOld.append(sceEmSet.getEnemyDirectionX()[1]);
                            emDataOld.append(sceEmSet.getEnemyDirectionY()[0]);
                            emDataOld.append(sceEmSet.getEnemyDirectionY()[1]);
                            emDataOld.append(sceEmSet.getEnemyDirectionZ()[0]);
                            emDataOld.append(sceEmSet.getEnemyDirectionZ()[1]);
                            System.out.println("EM DATA OLD: " + emDataOld.toString());

                        }
                    }
                    System.out.println("EM DATA NEW: " + emDataNew.toString());
                    List<String> oldEnemyData = new ArrayList<>();
                    List<String> newEnemyData = new ArrayList<>();

                    for(int i = 0; i < emDataOld.toString().length(); i++) {
                        if(i+1 < emDataOld.toString().length()) {
                            oldEnemyData.add(emDataOld.toString().substring(i, i + 2));
                            i++;
                        }
                    }
                    for(int i = 0; i < emDataNew.toString().length(); i++) {
                        if(i+1 < emDataNew.toString().length()) {
                            newEnemyData.add(emDataNew.toString().substring(i, i + 2));
                            i++;
                        }
                    }

                    int startPositionOfData = 0;
                    boolean dataFound = false;
                    for(int i = 0; i < scdDataNew.size(); i++) {
                        if(scdDataNew.get(i).equals(oldEnemyData.get(0))) {
                            startPositionOfData = i;
                            dataFound = true;
                            for(int j=0; j < oldEnemyData.size(); j++) {
                                if(oldEnemyData.get(j).equals(scdDataNew.get(i+j))) {}
                                else {
                                    dataFound = false;
                                }
                            }
                            if(dataFound == true) {
                                System.out.println("POSITION IN SCD DATA FOUND - Startposition - " + startPositionOfData);
                                break;
                            }
                        }
                    }

                    for(int i = 0; i < newEnemyData.size(); i++) {
                        scdDataNew.set(startPositionOfData + i, newEnemyData.get(i));
                    }

                    emDataOld = new StringBuilder();
                    emDataNew = new StringBuilder();
                }

                List<String> RDTList = RdtProcessing.getInstance().getRdtList();
                int scdStartData = RdtProcessing.getInstance().getScdDataStart();

                for(int i = 0; i < scdDataNew.size(); i++) {
                    RDTList.set(scdStartData+i, scdDataNew.get(i));
                }
                File RDTFile = new File(pathToRDT);
                RDTFile.delete();

                RdtProcessing.getInstance().convertHexToFile(RDTFile.getAbsolutePath(), RDTList);

                File ParentOfRDTFile = new File(RDTFile.getParent());
                ParentOfRDTFile = new File(ParentOfRDTFile.getParent());
                File extractedImageDirectory = new File(ParentOfRDTFile.getParent());
                File TempSubDirectory = new File(extractedImageDirectory.getParent());


                File dir = new File(TempSubDirectory.getAbsolutePath());
                File [] files = dir.listFiles(new FilenameFilter() {
                    @Override
                    public boolean accept(File dir, String name) {
                        return name.endsWith(".cat");
                    }
                });

                File catName = null;

                for (File catFile : files) {
                    catName = new File(catFile.getAbsolutePath());
                }

                System.out.println("IMAGE DIR: " + extractedImageDirectory.getAbsolutePath());
                System.out.println("CAT NAME: " + catName.getAbsolutePath());

                extractedImageDirectory.renameTo(new File(catName.getAbsolutePath().replace(".cat","")));

                imageUtilities.packImage();

            }
        } );
    }

    private void fetchElementsByClass(Map<String, List<Object>> finalListOfScdElements) {
        for ( String key : finalListOfScdElements.keySet() ) {
            System.out.println( key );
            List<Object> scdClassList = finalListOfScdElements.get(key);
            for(int i = 0; i < scdClassList.size(); i++) {
                fetchElementBySingleClass(scdClassList.get(i));
            }
        }
    }

    private void fetchElementBySingleClass(Object scdClassObject) {
        //Initialize the Buttons for the bottom
        JPanel elementPane = new JPanel();

        if(scdClassObject instanceof Enemy_sceEmSet_44) {
            Enemy_sceEmSet_44 sceEmSet = (Enemy_sceEmSet_44) scdClassObject;
            int enemyCounter = sceEmSet.getEnemyCounterDecimal();
            String enemyOpcode = sceEmSet.getOPCODE();
            componentList = new ArrayList<>();

            componentList.add(sceEmSet);

            JLabel label = new JLabel("Enemy Counter");
            elementPane.add(label);

            JTextField textField = new JTextField(String.valueOf(sceEmSet.getEnemyCounterDecimal()));
            textField.setName("EnemyCounter");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Enemy Type");
            elementPane.add(label);

            System.out.println(enemyMap.get(sceEmSet.getEnemyType()[0]));

            JComboBox enemyComboBox = fillEnemyComboBox();
            enemyComboBox.setSelectedItem(enemyMap.get(sceEmSet.getEnemyType()[0]));
            enemyComboBox.setName("EnemyType");
            elementPane.add(enemyComboBox);
            componentList.add(enemyComboBox);

            label = new JLabel("State");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemyStateDecimal()));
            textField.setName("State");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Unknown");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemyUnknownDecimal()));
            textField.setName("Unknown");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Soundbank");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemySoundBankDecimal()));
            textField.setName("Soundbank");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Texture");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemyTextureDecimal()));
            textField.setName("Texture");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Killed ID");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemyKilledIdDecimal()));
            textField.setName("KilledId");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Position X");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemyPositionXDecimal()));
            textField.setName("PositionX");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Position Y");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemyPositionYDecimal()));
            textField.setName("PositionY");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Position Z");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemyPositionZDecimal()));
            textField.setName("PositionZ");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Direction X");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemyDirectionXDecimal()));
            textField.setName("DirectionX");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Direction Y");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemyDirectionYDecimal()));
            textField.setName("DirectionY");
            elementPane.add(textField);
            componentList.add(textField);

            label = new JLabel("Direction Z");
            elementPane.add(label);

            textField = new JTextField(String.valueOf(sceEmSet.getEnemyDirectionZDecimal()));
            textField.setName("DirectionZ");
            elementPane.add(textField);
            componentList.add(textField);

            elementPane.add(new JSeparator(JSeparator.HORIZONTAL));

            elementPane.setLayout(new GridLayout(elementPane.getComponentCount(),0));
            elementFields.add(elementPane);
            componentMap.put(enemyOpcode + "-" + enemyCounter, componentList);
        }
    }

    private JComboBox fillEnemyComboBox() {
        String[] enemies = {  enemyMap.get("10"), enemyMap.get("11"), enemyMap.get("16"), enemyMap.get("20"), enemyMap.get("21"),
                            enemyMap.get("25"), enemyMap.get("27"), enemyMap.get("47") };

        return new JComboBox(enemies);
    }
}
