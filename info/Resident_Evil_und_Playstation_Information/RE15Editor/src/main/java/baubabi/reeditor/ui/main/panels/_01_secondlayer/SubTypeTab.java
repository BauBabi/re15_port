package baubabi.reeditor.ui.main.panels._01_secondlayer;

import baubabi.reeditor.core.general.utils.FileUtilities;
import baubabi.reeditor.core.general.utils.ProcessUtilities;
import baubabi.reeditor.core.image.data.Image;
import baubabi.reeditor.core.rdt.RdtProcessing;
import baubabi.reeditor.core.rdt._30_camera_rid.CameraRid30;
import baubabi.reeditor.core.rdt._33_zone_rvd.ZoneRvd33;
import baubabi.reeditor.core.rdt._34_light_lit.LightLit34;
import baubabi.reeditor.core.rdt._35_sprite_pri.SpritePri35;
import baubabi.reeditor.core.rdt._36_collision_sca.CollisionSca36;
import baubabi.reeditor.core.rdt._37_block_blk.BlockBlk37;
import baubabi.reeditor.core.rdt._38_floor_flr.FloorFlr38;
import baubabi.reeditor.core.rdt._40_main_scd.MainScd40;
import baubabi.reeditor.core.rdt._42_sub_scd.SubScd42;
import baubabi.reeditor.core.rdt._44_extra_scd.ExtraScd44;
import baubabi.reeditor.core.rdt._46_main_msg.MainMsg46;
import baubabi.reeditor.core.rdt._47_animation_rbj.AnimationRbj47;
import baubabi.reeditor.core.rdt._48_model_md1.ModelMd148;
import baubabi.reeditor.core.rdt._49_snd0_edt.Snd0Edt49;
import baubabi.reeditor.core.rdt._50_snd0_vh.Snd0Vh50;
import baubabi.reeditor.core.rdt._51_snd1_edt.Snd1Edt51;
import baubabi.reeditor.core.rdt._52_snd1_vh.Snd1Vh52;
import baubabi.reeditor.core.rdt._53_snd0_vb.Snd0Vb53;
import baubabi.reeditor.core.rdt._54_snd1_vb.Snd1Vb54;
import baubabi.reeditor.core.rdt._55_snd0_snd.Snd0Snd55;
import baubabi.reeditor.core.rdt._56_snd1_snd.Snd1Snd56;
import baubabi.reeditor.core.rdt._57_model_tim.ModelTim57;
import baubabi.reeditor.core.rdt._58_effect_esp.EffectEsp58;
import baubabi.reeditor.core.rdt._59_esp_tim.EspTim59;
import baubabi.reeditor.core.rdt._60_unknown_data.UnknownData60;
import baubabi.reeditor.ui.main.panels._02_thirdlayer.InfoDetailTab;
import baubabi.reeditor.ui.main.panels._02_thirdlayer.WipPage;
import lombok.Getter;

import javax.swing.*;
import java.awt.*;
import java.io.File;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

@Getter
public class SubTypeTab {

    private final JTabbedPane tabbedPane = new JTabbedPane();

    private final RdtProcessing rdtProcessing;

    private final FileUtilities fileUtilities;

    private final Image image;

    private Component selectionComponent;

    public SubTypeTab(String selectedRdtItem) {
        this.fileUtilities = FileUtilities.getInstance();
        this.rdtProcessing = RdtProcessing.getInstance();
        this.image = Image.getInstance();

        File stageDirectoryForRdt = new File(rdtProcessing.getCurrentProcessedRdt().getParent());

        splitImagesForRdt(selectedRdtItem, stageDirectoryForRdt);

        createImagesFromBssFiles();

        assignImagesToRdtProcessing();

        List<String> rdtTabs = fetchRdtTabTexts();

        for (int i = 0; i < rdtTabs.size(); i++) {
            JComponent panel = makeTextPanel(rdtTabs.get(i));
            tabbedPane.addTab(rdtTabs.get(i), null, panel, "Element Panel");
        }

        if(rdtTabs.size() == 0) {
            //Sub Panel for the Tab
            JPanel panel = new JPanel(false);
            panel.setLayout(new BorderLayout());

            //Text for the tab pane - CENTERED
            JLabel filler = new JLabel("No Elements for this RDT available");
            filler.setHorizontalAlignment(JLabel.CENTER);
            panel.add(filler);
            selectionComponent = panel;

            tabbedPane.addTab("No Elements for this RDT available", null, panel, "Element Panel");
        }

        tabbedPane.setSelectedComponent(selectionComponent);
        tabbedPane.setVisible(true);
    }

    protected JComponent makeTextPanel(String text) {
        //Sub Panel for the Tab
        JPanel panel = new JPanel(false);
        panel.setLayout(new BorderLayout());

        //Text for the tab pane - CENTERED
        JLabel filler = new JLabel(text);
        filler.setHorizontalAlignment(JLabel.CENTER);
        panel.add(filler);

        if (text.equals("SCD")) {
            selectionComponent = panel;

            //Adds next Layer of tabs
            InfoDetailTab infoDetailTab = new InfoDetailTab();

            panel.add(infoDetailTab.getTabbedPane());
        } else {
            WipPage wipPage = new WipPage();
            panel.add(wipPage.getPanel());
        }

        return panel;
    }

    private List<String> fetchRdtTabTexts() {

        List<Object> rdtObjectList = rdtProcessing.getRdtObjectList();

        return fetchTextFromRdtClasses(rdtObjectList);
    }

    private List<String> fetchTextFromRdtClasses(List<Object> rdtObjectList) {
        List<String> rdtList = new ArrayList<>();

        boolean cameraRidSet = false;
        boolean zoneRvdSet = false;
        boolean lightLitSet = false;
        boolean spritePriSet = false;
        boolean collisionScaSet = false;
        boolean blockBlkSet = false;
        boolean floorFlrSet = false;
        boolean scdSet = false;
        boolean msgSet = false;
        boolean animationRbjSet = false;
        boolean modelSet = false;
        boolean effectEspSet = false;
        boolean snd0Set = false;
        boolean snd1Set = false;
        boolean espTimSet = false;
        boolean unknownDataSet = false;

        int count = 0;

        for (int i = 0; i < rdtObjectList.size(); i++) {
            if (rdtObjectList.get(i) instanceof CameraRid30) {
                if (!cameraRidSet) {
                    rdtList.add("camera.rid");
                    cameraRidSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof ZoneRvd33) {
                if (!zoneRvdSet) {
                    rdtList.add("zone.rvd");
                    zoneRvdSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof LightLit34) {
                if (!lightLitSet) {
                    rdtList.add("light.lit");
                    lightLitSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof SpritePri35) {
                if (!spritePriSet) {
                    rdtList.add("sprite.pri");
                    spritePriSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof CollisionSca36) {
                if (!collisionScaSet) {
                    rdtList.add("collision.sca");
                    collisionScaSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof BlockBlk37) {
                if (!blockBlkSet) {
                    rdtList.add("block.blk");
                    blockBlkSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof FloorFlr38) {
                if (!floorFlrSet) {
                    rdtList.add("floor.flr");
                    floorFlrSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof MainScd40 || rdtObjectList.get(i) instanceof SubScd42 || rdtObjectList.get(i) instanceof ExtraScd44) {
                if (!scdSet) {
                    rdtList.add("SCD");
                    scdSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof MainMsg46) {
                if (!msgSet) {
                    rdtList.add("MSG");
                    msgSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof AnimationRbj47) {
                if (!animationRbjSet) {
                    rdtList.add("animation.rbj");
                    animationRbjSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof ModelMd148 || rdtObjectList.get(i) instanceof ModelTim57) {
                if (!modelSet) {
                    rdtList.add("MODEL");
                    modelSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof EffectEsp58) {
                if (!effectEspSet) {
                    rdtList.add("effect.esp");
                    effectEspSet = true;
                    count = count + 1;
                }
            }

            else if (rdtObjectList.get(i) instanceof Snd0Edt49 || rdtObjectList.get(i) instanceof Snd0Vh50 || rdtObjectList.get(i) instanceof Snd0Vb53 || rdtObjectList.get(i) instanceof Snd0Snd55) {
                if (!snd0Set) {
                    rdtList.add("SND0");
                    snd0Set = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof Snd1Edt51 || rdtObjectList.get(i) instanceof Snd1Vh52 || rdtObjectList.get(i) instanceof Snd1Vb54 || rdtObjectList.get(i) instanceof Snd1Snd56) {
                if (!snd1Set) {
                    rdtList.add("SND1");
                    snd1Set = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof EspTim59) {
                if (!espTimSet) {
                    rdtList.add("esp.tim");
                    espTimSet = true;
                    count = count + 1;
                }
            } else if (rdtObjectList.get(i) instanceof UnknownData60) {
                if (!unknownDataSet) {
                    rdtList.add("unknown data");
                    unknownDataSet = true;
                    count = count + 1;
                }
            }
            else if (count == 16) {
                break;
            }
        }

        return rdtList;
    }

    private void splitImagesForRdt(String selectedRdt, File stageDirectory) {
        String bssFileString = selectedRdt.replace(".RDT", "");
        bssFileString = bssFileString.substring(0, bssFileString.length() - 1);
        bssFileString = bssFileString + ".BSS";

        List<String> bssFileAsHex = this.fileUtilities.readFileAsHex(Paths.get(stageDirectory.getAbsolutePath() + File.separator + bssFileString));

        this.fileUtilities.splitHexListToAbsoluteFileByByteSize(bssFileAsHex, 65536, image.getWorkDirectory() + File.separator + "BSS2BMP" + File.separator + bssFileString);
    }

    private void createImagesFromBssFiles() {
        File bssDirectory = new File(image.getWorkDirectory() + File.separator + "BSS2BMP");
        File[] imageBssFiles = this.fileUtilities.searchForFilesInDirectoryWithWildcard(bssDirectory, "*.BSS");
        ProcessUtilities.getInstance().extractBackgroundImages(imageBssFiles, bssDirectory);
    }

    private void assignImagesToRdtProcessing() {
        File bssDirectory = new File(image.getWorkDirectory() + File.separator + "BSS2BMP");
        File[] imageBmpFiles = this.fileUtilities.searchForFilesInDirectoryWithWildcard(bssDirectory, "*.bmp");
        rdtProcessing.setCurrentProcessedBackgroundImage(imageBmpFiles);
    }
}
