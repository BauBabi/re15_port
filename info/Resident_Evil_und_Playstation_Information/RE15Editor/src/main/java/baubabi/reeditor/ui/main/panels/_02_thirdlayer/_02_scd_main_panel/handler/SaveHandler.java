package baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler;

import baubabi.reeditor.core.general.utils.FileUtilities;
import baubabi.reeditor.core.general.utils.ReflectionUtilities;
import baubabi.reeditor.core.image.utils.ImageUtilities;
import baubabi.reeditor.core.rdt.RdtProcessing;
import baubabi.reeditor.core.rdt._00_sprite.Sprite00;
import baubabi.reeditor.core.rdt._01_cut_amount_cameras_room.Cut01;
import baubabi.reeditor.core.rdt._02_amount_md1_room.Md1Room02;
import baubabi.reeditor.core.rdt._03_items.Items03;
import baubabi.reeditor.core.rdt._04_door.Door04;
import baubabi.reeditor.core.rdt._05_room_at.RoomAt05;
import baubabi.reeditor.core.rdt._06_reverb_lv.ReverbLv06;
import baubabi.reeditor.core.rdt._07_pri_sprite_max_room.PriSpriteRoomMax07;
import baubabi.reeditor.core.rdt._08_snd0_edt_address.Snd0EdtAddress08;
import baubabi.reeditor.core.rdt._09_snd0_vh_address.Snd0VhAddress09;
import baubabi.reeditor.core.rdt._10_snd0_vb_address.Snd0VbAddress10;
import baubabi.reeditor.core.rdt._11_snd1_edt_address.Snd1EdtAddress11;
import baubabi.reeditor.core.rdt._12_snd1_vh_address.Snd1VhAddress12;
import baubabi.reeditor.core.rdt._13_snd1_vb_address.Snd1VbAddress13;
import baubabi.reeditor.core.rdt._14_collision_sca_address.CollisionScaAddress14;
import baubabi.reeditor.core.rdt._15_camera_rid_address.CameraRidAddress15;
import baubabi.reeditor.core.rdt._16_zone_rvd_address.ZoneRvdAddress16;
import baubabi.reeditor.core.rdt._17_light_lit_address.LightLitAddress17;
import baubabi.reeditor.core.rdt._18_md1_tim_pointer_address.Md1TimPointerAddress18;
import baubabi.reeditor.core.rdt._19_floor_flr_address.FloorFlrAddress19;
import baubabi.reeditor.core.rdt._20_block_blk_address.BlockBlkAddress20;
import baubabi.reeditor.core.rdt._21_message_msg_address.MessageMsgAddress21;
import baubabi.reeditor.core.rdt._22_main_scd_offset_address.MainScdOffsetAddress22;
import baubabi.reeditor.core.rdt._23_sub_scd_offset_address.SubScdOffsetAddress23;
import baubabi.reeditor.core.rdt._24_extra_scd_offset_address.ExtraScdOffsetAddress24;
import baubabi.reeditor.core.rdt._25_effect_esp_address.EffectEspAddress25;
import baubabi.reeditor.core.rdt._26_something_evtl_eff_address.SomethingEvtlEffAddress26;
import baubabi.reeditor.core.rdt._27_esp_tim_address.EspTimAddress27;
import baubabi.reeditor.core.rdt._28_model_tim_address.ModelTimAddress28;
import baubabi.reeditor.core.rdt._29_animation_rbj_address.AnimationRbjAddress29;
import baubabi.reeditor.core.rdt._30_camera_rid.CameraRid30;
import baubabi.reeditor.core.rdt._31_model_tim_element_address.ModelTimElementAddress31;
import baubabi.reeditor.core.rdt._32_model_md1_element_address.ModelMd1ElementAddress32;
import baubabi.reeditor.core.rdt._33_zone_rvd.ZoneRvd33;
import baubabi.reeditor.core.rdt._34_light_lit.LightLit34;
import baubabi.reeditor.core.rdt._35_sprite_pri.SpritePri35;
import baubabi.reeditor.core.rdt._36_collision_sca.CollisionSca36;
import baubabi.reeditor.core.rdt._37_block_blk.BlockBlk37;
import baubabi.reeditor.core.rdt._38_floor_flr.FloorFlr38;
import baubabi.reeditor.core.rdt._39_main_scd_offset.MainScdOffset39;
import baubabi.reeditor.core.rdt._40_main_scd.MainScd40;
import baubabi.reeditor.core.rdt._41_sub_scd_offset.SubScdOffset41;
import baubabi.reeditor.core.rdt._42_sub_scd.SubScd42;
import baubabi.reeditor.core.rdt._43_extra_scd_offset.ExtraScdOffset43;
import baubabi.reeditor.core.rdt._44_extra_scd.ExtraScd44;
import baubabi.reeditor.core.rdt._45_main_msg_offset.MainMsgOffset45;
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
import baubabi.reeditor.core.rdt.scd._68_op44_sce_em_set.Enemy_sceEmSet_44;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.ScdMainPanel;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.save_object._44_SceEmSet;
import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.utilities.HandlerUtilities;
import com.google.common.base.Splitter;
import com.google.common.collect.Lists;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class SaveHandler {
    public SaveHandler(ScdMainPanel scdPanel, Map<String, Object> scdMap) {
        HashMap<Object, Object> valuesToSave = scdPanel.getOldValues();
        for (Map.Entry<Object, Object> entry : valuesToSave.entrySet()) {
            Object currentCorrespondingScdObjectClass = entry.getValue();

            List<Object> scdPanelElements = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnObjectList(currentCorrespondingScdObjectClass, "getComponentList", this.getClass().getCanonicalName());

            updateScdElements(scdPanelElements);
        }

        updateFullScdHexByteData(scdMap);


        String finalHexStringForSave = getFinalHexStringFromRdtElementsForSave();

        ArrayList<String> finalHexForSaveListForNewRdtFile = createNewRdtListFromHexForSaveString(finalHexStringForSave);


        //ONLY FOR DEBUG
        //printOldAndNewListContent(finalHexStringForSave);

        replaceRdtObjectListWithNewRdtList(finalHexForSaveListForNewRdtFile);


        backupOldPsxImage();

        deleteOldRdtFile();

        writeNewRdtFile(finalHexForSaveListForNewRdtFile);

        createNewPsxImage();
    }

    public void updateScdElements(List<Object> scdPanelElements) {
        if(scdPanelElements.size() > 0) {
            Object scdElementType = scdPanelElements.get(0);

            if (scdElementType instanceof Enemy_sceEmSet_44) {
                _44_SceEmSet sceEmSet = new _44_SceEmSet();
                sceEmSet.saveEnemyData((Enemy_sceEmSet_44) scdElementType, scdPanelElements);
            }
        }
    }

    public void updateFullScdHexByteData(Map<String, Object> scdMap) {
        for (Map.Entry<String,Object> entry : scdMap.entrySet()) {
            if(scdMap.get(entry.getKey()) instanceof MainScd40) {
                MainScd40 mainScd40 = (MainScd40) scdMap.get(entry.getKey());

                List<Object> singleScdDataElements = mainScd40.getSingleScdDataElements();
                String newMainScdData = HandlerUtilities.getInstance().fetchAndReplaceScdElementData(singleScdDataElements);
                mainScd40.setScdData(newMainScdData);
            }
            else if(scdMap.get(entry.getKey()) instanceof SubScd42) {
                SubScd42 subScd42 = (SubScd42) scdMap.get(entry.getKey());

                List<Object> singleScdDataElements = subScd42.getSingleScdDataElements();
                String newSubScdData = HandlerUtilities.getInstance().fetchAndReplaceScdElementData(singleScdDataElements);
                subScd42.setScdData(newSubScdData);
            }
            else if(scdMap.get(entry.getKey()) instanceof ExtraScd44) {
                ExtraScd44 extraScd44 = (ExtraScd44) scdMap.get(entry.getKey());

                List<Object> singleScdDataElements = extraScd44.getSingleScdDataElements();
                String newExtraScdData = HandlerUtilities.getInstance().fetchAndReplaceScdElementData(singleScdDataElements);
                extraScd44.setScdData(newExtraScdData);
            }
        }
    }

    private String getFinalHexStringFromRdtElementsForSave() {
        List<Object> rdtObjectListToSave = RdtProcessing.getInstance().getRdtObjectList();
        String finalHexForSave = "";
        for(int i = 0; i < rdtObjectListToSave.size(); i++) {
            Object currentRdtObject = rdtObjectListToSave.get(i);
            if(currentRdtObject instanceof Sprite00) {
                Sprite00 sprite00 = (Sprite00) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", sprite00.getSprite()), sprite00.getElementSize()));
            }
            else if(currentRdtObject instanceof Cut01) {
                Cut01 cut01 = (Cut01) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", cut01.getCut()), cut01.getElementSize()));
            }
            else if(currentRdtObject instanceof Md1Room02) {
                Md1Room02 md1Room02 = (Md1Room02) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", md1Room02.getMd1()), md1Room02.getElementSize()));
            }
            else if(currentRdtObject instanceof Items03) {
                Items03 items03 = (Items03) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", items03.getItems()), items03.getElementSize()));
            }
            else if(currentRdtObject instanceof Door04) {
                Door04 door04 = (Door04) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", door04.getDoor()), door04.getElementSize()));
            }
            else if(currentRdtObject instanceof RoomAt05) {
                RoomAt05 roomAt05 = (RoomAt05) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", roomAt05.getRoomAt()), roomAt05.getElementSize()));
            }
            else if(currentRdtObject instanceof ReverbLv06) {
                ReverbLv06 reverbLv06 = (ReverbLv06) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", reverbLv06.getReverbLv()), reverbLv06.getElementSize()));
            }
            else if(currentRdtObject instanceof PriSpriteRoomMax07) {
                PriSpriteRoomMax07 priSpriteRoomMax07 = (PriSpriteRoomMax07) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", priSpriteRoomMax07.getPriSpriteRoomMax()), priSpriteRoomMax07.getElementSize()));
            }
            else if(currentRdtObject instanceof Snd0EdtAddress08) {
                Snd0EdtAddress08 snd0EdtAddress08 = (Snd0EdtAddress08) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", snd0EdtAddress08.getAddress()), snd0EdtAddress08.getElementSize()));
            }
            else if(currentRdtObject instanceof Snd0VhAddress09) {
                Snd0VhAddress09 snd0VhAddress09 = (Snd0VhAddress09) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", snd0VhAddress09.getAddress()), snd0VhAddress09.getElementSize()));
            }
            else if(currentRdtObject instanceof Snd0VbAddress10) {
                Snd0VbAddress10 snd0VbAddress10 = (Snd0VbAddress10) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", snd0VbAddress10.getAddress()), snd0VbAddress10.getElementSize()));
            }
            else if(currentRdtObject instanceof Snd1EdtAddress11) {
                Snd1EdtAddress11 snd1EdtAddress11 = (Snd1EdtAddress11) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", snd1EdtAddress11.getAddress()), snd1EdtAddress11.getElementSize()));
            }
            else if(currentRdtObject instanceof Snd1VhAddress12) {
                Snd1VhAddress12 snd1VhAddress12 = (Snd1VhAddress12) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", snd1VhAddress12.getAddress()), snd1VhAddress12.getElementSize()));
            }
            else if(currentRdtObject instanceof Snd1VbAddress13) {
                Snd1VbAddress13 snd1VbAddress13 = (Snd1VbAddress13) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", snd1VbAddress13.getAddress()), snd1VbAddress13.getElementSize()));
            }
            else if(currentRdtObject instanceof CollisionScaAddress14) {
                CollisionScaAddress14 collisionScaAddress14 = (CollisionScaAddress14) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", collisionScaAddress14.getAddress()), collisionScaAddress14.getElementSize()));
            }
            else if(currentRdtObject instanceof CameraRidAddress15) {
                CameraRidAddress15 cameraRidAddress15 = (CameraRidAddress15) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", cameraRidAddress15.getAddress()), cameraRidAddress15.getElementSize()));
            }
            else if(currentRdtObject instanceof ZoneRvdAddress16) {
                ZoneRvdAddress16 zoneRvdAddress16 = (ZoneRvdAddress16) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", zoneRvdAddress16.getAddress()), zoneRvdAddress16.getElementSize()));
            }
            else if(currentRdtObject instanceof LightLitAddress17) {
                LightLitAddress17 lightLitAddress17 = (LightLitAddress17) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", lightLitAddress17.getAddress()), lightLitAddress17.getElementSize()));
            }
            else if(currentRdtObject instanceof Md1TimPointerAddress18) {
                Md1TimPointerAddress18 md1TimPointerAddress18 = (Md1TimPointerAddress18) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", md1TimPointerAddress18.getAddress()), md1TimPointerAddress18.getElementSize()));
            }
            else if(currentRdtObject instanceof FloorFlrAddress19) {
                FloorFlrAddress19 floorFlrAddress19 = (FloorFlrAddress19) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", floorFlrAddress19.getAddress()), floorFlrAddress19.getElementSize()));
            }
            else if(currentRdtObject instanceof BlockBlkAddress20) {
                BlockBlkAddress20 blockBlkAddress20 = (BlockBlkAddress20) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", blockBlkAddress20.getAddress()), blockBlkAddress20.getElementSize()));
            }
            else if(currentRdtObject instanceof MessageMsgAddress21) {
                MessageMsgAddress21 messageMsgAddress21 = (MessageMsgAddress21) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", messageMsgAddress21.getAddress()), messageMsgAddress21.getElementSize()));
            }
            else if(currentRdtObject instanceof MainScdOffsetAddress22) {
                MainScdOffsetAddress22 mainScdOffsetAddress22 = (MainScdOffsetAddress22) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", mainScdOffsetAddress22.getAddress()), mainScdOffsetAddress22.getElementSize()));
            }
            else if(currentRdtObject instanceof SubScdOffsetAddress23) {
                SubScdOffsetAddress23 subScdOffsetAddress23 = (SubScdOffsetAddress23) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", subScdOffsetAddress23.getAddress()), subScdOffsetAddress23.getElementSize()));
            }
            else if(currentRdtObject instanceof ExtraScdOffsetAddress24) {
                ExtraScdOffsetAddress24 extraScdOffsetAddress24 = (ExtraScdOffsetAddress24) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", extraScdOffsetAddress24.getAddress()), extraScdOffsetAddress24.getElementSize()));
            }
            else if(currentRdtObject instanceof EffectEspAddress25) {
                EffectEspAddress25 effectEspAddress25 = (EffectEspAddress25) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", effectEspAddress25.getAddress()), effectEspAddress25.getElementSize()));
            }
            else if(currentRdtObject instanceof SomethingEvtlEffAddress26) {
                SomethingEvtlEffAddress26 somethingEvtlEffAddress26 = (SomethingEvtlEffAddress26) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", somethingEvtlEffAddress26.getAddress()), somethingEvtlEffAddress26.getElementSize()));
            }
            else if(currentRdtObject instanceof EspTimAddress27) {
                EspTimAddress27 espTimAddress27 = (EspTimAddress27) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", espTimAddress27.getAddress()), espTimAddress27.getElementSize()));
            }
            else if(currentRdtObject instanceof ModelTimAddress28) {
                ModelTimAddress28 modelTimAddress28 = (ModelTimAddress28) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", modelTimAddress28.getAddress()), modelTimAddress28.getElementSize()));
            }
            else if(currentRdtObject instanceof AnimationRbjAddress29) {
                AnimationRbjAddress29 animationRbjAddress29 = (AnimationRbjAddress29) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", animationRbjAddress29.getAddress()), animationRbjAddress29.getElementSize()));
            }
            else if(currentRdtObject instanceof CameraRid30) {
                CameraRid30 cameraRid30 = (CameraRid30) currentRdtObject;
                finalHexForSave = finalHexForSave + cameraRid30.getCameraRidData();
            }
            else if(currentRdtObject instanceof ModelTimElementAddress31) {
                ModelTimElementAddress31 modelTimElementAddress31 = (ModelTimElementAddress31) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", modelTimElementAddress31.getAddress()), modelTimElementAddress31.getElementSize()));
            }
            else if(currentRdtObject instanceof ModelMd1ElementAddress32) {
                ModelMd1ElementAddress32 modelMd1ElementAddress32 = (ModelMd1ElementAddress32) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", modelMd1ElementAddress32.getAddress()), modelMd1ElementAddress32.getElementSize()));
            }
            else if(currentRdtObject instanceof ZoneRvd33) {
                ZoneRvd33 zoneRvd33 = (ZoneRvd33) currentRdtObject;
                finalHexForSave = finalHexForSave + zoneRvd33.getZoneRvdData();
            }
            else if(currentRdtObject instanceof LightLit34) {
                LightLit34 lightLit34 = (LightLit34) currentRdtObject;
                finalHexForSave = finalHexForSave + lightLit34.getLightLitData();
            }
            else if(currentRdtObject instanceof SpritePri35) {
                SpritePri35 spritePri35 = (SpritePri35) currentRdtObject;
                finalHexForSave = finalHexForSave + spritePri35.getSpritePriData();
            }
            else if(currentRdtObject instanceof CollisionSca36) {
                CollisionSca36 collisionSca36 = (CollisionSca36) currentRdtObject;
                finalHexForSave = finalHexForSave + collisionSca36.getCollisionScaData();
            }
            else if(currentRdtObject instanceof BlockBlk37) {
                BlockBlk37 blockBlk37 = (BlockBlk37) currentRdtObject;
                finalHexForSave = finalHexForSave + blockBlk37.getBlockBlkData();
            }
            else if(currentRdtObject instanceof FloorFlr38) {
                FloorFlr38 floorFlr38 = (FloorFlr38) currentRdtObject;
                finalHexForSave = finalHexForSave + floorFlr38.getFloorFlrData();
            }
            else if(currentRdtObject instanceof MainScdOffset39) {
                MainScdOffset39 mainScdOffset39 = (MainScdOffset39) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", mainScdOffset39.getCorrespondingScdOffset()), mainScdOffset39.getElementSize()));
            }
            else if(currentRdtObject instanceof MainScd40) {
                MainScd40 mainScd40 = (MainScd40) currentRdtObject;
                finalHexForSave = finalHexForSave + mainScd40.getScdData();
            }
            else if(currentRdtObject instanceof SubScdOffset41) {
                SubScdOffset41 subScdOffset41 = (SubScdOffset41) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", subScdOffset41.getCorrespondingScdOffset()), subScdOffset41.getElementSize()));
            }
            else if(currentRdtObject instanceof SubScd42) {
                SubScd42 subScd42 = (SubScd42) currentRdtObject;
                finalHexForSave = finalHexForSave + subScd42.getScdData();
            }
            else if(currentRdtObject instanceof ExtraScdOffset43) {
                ExtraScdOffset43 extraScdOffset43 = (ExtraScdOffset43) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", extraScdOffset43.getCorrespondingScdOffset()), extraScdOffset43.getElementSize()));
            }
            else if(currentRdtObject instanceof ExtraScd44) {
                ExtraScd44 extraScd44 = (ExtraScd44) currentRdtObject;
                finalHexForSave = finalHexForSave + extraScd44.getScdData();
            }
            else if(currentRdtObject instanceof MainMsgOffset45) {
                MainMsgOffset45 mainMsgOffset45 = (MainMsgOffset45) currentRdtObject;
                finalHexForSave = finalHexForSave + HandlerUtilities.getInstance().reverseHex(HandlerUtilities.getInstance().filler(String.format("%02X", mainMsgOffset45.getMainMsgOffset()), mainMsgOffset45.getElementSize()));
            }
            else if(currentRdtObject instanceof MainMsg46) {
                MainMsg46 mainMsg46 = (MainMsg46) currentRdtObject;
                finalHexForSave = finalHexForSave + mainMsg46.getMainMsgData();
            }
            else if(currentRdtObject instanceof AnimationRbj47) {
                AnimationRbj47 animationRbj47 = (AnimationRbj47) currentRdtObject;
                finalHexForSave = finalHexForSave + animationRbj47.getAnimationRbjData();
            }
            else if(currentRdtObject instanceof ModelMd148) {
                ModelMd148 modelMd148 = (ModelMd148) currentRdtObject;
                finalHexForSave = finalHexForSave + modelMd148.getModelMd1Data();
            }
            else if(currentRdtObject instanceof EffectEsp58) {
                EffectEsp58 effectEsp58 = (EffectEsp58) currentRdtObject;
                finalHexForSave = finalHexForSave + effectEsp58.getEffectEspData();
            }
            else if(currentRdtObject instanceof Snd0Edt49) {
                Snd0Edt49 snd0Edt49 = (Snd0Edt49) currentRdtObject;
                finalHexForSave = finalHexForSave + snd0Edt49.getSnd0EdtData();
            }
            else if(currentRdtObject instanceof Snd0Vh50) {
                Snd0Vh50 snd0Vh50 = (Snd0Vh50) currentRdtObject;
                finalHexForSave = finalHexForSave + snd0Vh50.getSnd0VhData();
            }
            else if(currentRdtObject instanceof Snd1Edt51) {
                Snd1Edt51 snd1Edt51 = (Snd1Edt51) currentRdtObject;
                finalHexForSave = finalHexForSave + snd1Edt51.getSnd1EdtData();
            }
            else if(currentRdtObject instanceof Snd1Vh52) {
                Snd1Vh52 snd1Vh52 = (Snd1Vh52) currentRdtObject;
                finalHexForSave = finalHexForSave + snd1Vh52.getSnd1VhData();
            }
            else if(currentRdtObject instanceof Snd0Vb53) {
                Snd0Vb53 snd0Vb53 = (Snd0Vb53) currentRdtObject;
                finalHexForSave = finalHexForSave + snd0Vb53.getSnd0VbData();
            }
            else if(currentRdtObject instanceof Snd1Vb54) {
                Snd1Vb54 snd1Vb54 = (Snd1Vb54) currentRdtObject;
                finalHexForSave = finalHexForSave + snd1Vb54.getSnd1VbData();
            }
            else if(currentRdtObject instanceof Snd0Snd55) {
                Snd0Snd55 snd0Snd55 = (Snd0Snd55) currentRdtObject;
                finalHexForSave = finalHexForSave + snd0Snd55.getSnd0SndData();
            }
            else if(currentRdtObject instanceof Snd1Snd56) {
                Snd1Snd56 snd1Snd56 = (Snd1Snd56) currentRdtObject;
                finalHexForSave = finalHexForSave + snd1Snd56.getSnd1SndData();
            }
            else if(currentRdtObject instanceof EspTim59) {
                EspTim59 espTim59 = (EspTim59) currentRdtObject;
                finalHexForSave = finalHexForSave + espTim59.getEspTimData();
            }
            else if(currentRdtObject instanceof UnknownData60) {
                UnknownData60 unknownData60 = (UnknownData60) currentRdtObject;
                finalHexForSave = finalHexForSave + unknownData60.getUnknownData();
            }
            else if(currentRdtObject instanceof ModelTim57) {
                ModelTim57 modelTim57 = (ModelTim57) currentRdtObject;
                finalHexForSave = finalHexForSave + modelTim57.getModelTimData();
            }
        }

        return finalHexForSave;
    }

    public ArrayList<String> createNewRdtListFromHexForSaveString(String finalHexForSaveString) {
        ArrayList<String> newRdtList = new ArrayList<String>();
        Iterable<String> splittedFinalList = Splitter.fixedLength(2).split(finalHexForSaveString);
        newRdtList = Lists.newArrayList(splittedFinalList);
        /*for(int i = 0; i < finalHexForSaveString.length(); i=i+2) {
            newRdtList.add(finalHexForSaveString.substring(i,i+2));
        }*/
        return newRdtList;
    }

    public void printOldAndNewListContent(String finalHexForSave) {
        List<String> rdtList = RdtProcessing.getInstance().getRdtList();
        String oldList = "";

        oldList = String.join("", rdtList);

        System.out.println("Old List before Saving: " + oldList);
        System.out.println("New List for Saving: " + finalHexForSave);
        RdtProcessing instance = RdtProcessing.getInstance();
        List<Object> rdtObjectList = instance.getRdtObjectList();
        System.out.println("OBJECT LIST " + rdtObjectList);
    }

    public void replaceRdtObjectListWithNewRdtList(ArrayList<String> newRdtList) {
        RdtProcessing.getInstance().setNewRdtList(newRdtList);
    }

    public void backupOldPsxImage() {
        //Backup file
        baubabi.reeditor.core.image.data.Image image = baubabi.reeditor.core.image.data.Image.getInstance();
        File imageWithDirectory = new File(image.getWorkDirectory() + File.separator + image.getImageName());
        FileUtilities.getInstance().copyFile(imageWithDirectory.getAbsolutePath(), imageWithDirectory.getAbsolutePath().replace(".bin","") + "_backup.bin");
    }

    public void deleteOldRdtFile() {
        File RDTFile = RdtProcessing.getInstance().getCurrentProcessedRdt();
        RDTFile.delete();
    }

    public void writeNewRdtFile(ArrayList<String> newRdtList) {
        FileUtilities.getInstance().writeHexAsFile(RdtProcessing.getInstance().getCurrentProcessedRdt().getAbsolutePath(), newRdtList);
    }

    public void createNewPsxImage() {
        ImageUtilities.getInstance().packImage();
    }
}
