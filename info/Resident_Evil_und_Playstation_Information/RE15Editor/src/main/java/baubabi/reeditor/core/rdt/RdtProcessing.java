package baubabi.reeditor.core.rdt;

import baubabi.reeditor.core.general.utils.DataUtilities;
import baubabi.reeditor.core.general.utils.FileUtilities;
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
import baubabi.reeditor.core.rdt._47_animation_rbj.AnimationRbj47;
import baubabi.reeditor.core.rdt._49_snd0_edt.Snd0Edt49;
import baubabi.reeditor.core.rdt._50_snd0_vh.Snd0Vh50;
import baubabi.reeditor.core.rdt._51_snd1_edt.Snd1Edt51;
import baubabi.reeditor.core.rdt._52_snd1_vh.Snd1Vh52;
import baubabi.reeditor.core.rdt._53_snd0_vb.Snd0Vb53;
import baubabi.reeditor.core.rdt._54_snd1_vb.Snd1Vb54;
import baubabi.reeditor.core.rdt._55_snd0_snd.Snd0Snd55;
import baubabi.reeditor.core.rdt._56_snd1_snd.Snd1Snd56;
import baubabi.reeditor.core.rdt._58_effect_esp.EffectEsp58;
import baubabi.reeditor.core.rdt._59_esp_tim.EspTim59;
import baubabi.reeditor.core.rdt._60_unknown_data.UnknownData60;
import baubabi.reeditor.core.rdt._61_unknown_data_address.UnknownDataAddress61;
import baubabi.reeditor.core.rdt._62_snd0_snd_address.Snd0SndAddress62;
import baubabi.reeditor.core.rdt._63_snd1_snd_address.Snd1SndAddress63;
import baubabi.reeditor.core.rdt._64_sprite_pri_address.SpritePriAddress64;
import baubabi.reeditor.core.rdt._65_esp_tim_element_address.EspTimElementAddress65;
import baubabi.reeditor.core.rdt.scd.ScdUtilities;
import lombok.Getter;
import lombok.Setter;
import lombok.extern.log4j.Log4j2;

import java.io.File;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

@Getter
@Setter
@Log4j2
public class RdtProcessing {

    private static RdtProcessing INSTANCE;

    private final FileUtilities fileUtilities;

    private final DataUtilities dataUtilities;

    private final RdtUtilities rdtUtilities;

    private final ScdUtilities scdUtilities;

    List<String> rdtList;

    List<Object> rdtObjectList;

    File[] currentProcessedBackgroundImage;

    List<Object> mainScdDataElements;

    List<Object> subScdDataElements;

    List<Object> extraScdDataElements;

    List<Object> allScdDataElements;

    HashMap<String, Integer> classPositionMapInOrder;

    ArrayList<Integer> classOffsetStartListInOrder;



    private File currentProcessedRdt;

    private RdtProcessing() {
        fileUtilities = FileUtilities.getInstance();
        dataUtilities = DataUtilities.getInstance();
        rdtUtilities = RdtUtilities.getInstance();
        scdUtilities = ScdUtilities.getInstance();
    }

    public static RdtProcessing getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new RdtProcessing();
        }

        return INSTANCE;
    }

    public void parseRdt(File pathToRdtFile) {
        currentProcessedRdt = pathToRdtFile;

        //READ IN RDT
        this.rdtList = fileUtilities.readFileAsHex(Paths.get(pathToRdtFile.getAbsolutePath()));
        log.info("RDT Bytes: " + rdtList.size());

        this.rdtObjectList = parseRdtDataElements(rdtList, pathToRdtFile.getAbsolutePath());
    }

    private List<Object> parseRdtDataElements(List<String> rdtList, String rdtFilePath) {
        List<Object> rdtDataCollectionList = new ArrayList<>();

        if(rdtList.size() > 96) {
            //00 Fetch sprite data
            Sprite00 sprite00 = new Sprite00(dataUtilities.convertHexToInt(rdtList.get(0)));
            rdtDataCollectionList.add(sprite00);

            //01 Fetch amount of cameras in room data
            Cut01 cut01 = new Cut01(dataUtilities.convertHexToInt(rdtList.get(1)));
            rdtDataCollectionList.add(cut01);

            //02 Fetch amount of md1 in room data
            Md1Room02 md1Room02 = new Md1Room02(dataUtilities.convertHexToInt(rdtList.get(2)));
            rdtDataCollectionList.add(md1Room02);

            //03 Fetch amount of items data
            Items03 items03 = new Items03(dataUtilities.convertHexToInt(rdtList.get(3)));
            rdtDataCollectionList.add(items03);

            //04 Fetch door data
            Door04 door04 = new Door04(dataUtilities.convertHexToInt(rdtList.get(4)));
            rdtDataCollectionList.add(door04);

            //05 Room At data
            RoomAt05 roomAt05 = new RoomAt05(dataUtilities.convertHexToInt(rdtList.get(5)));
            rdtDataCollectionList.add(roomAt05);

            //06 Reverb Lb data
            ReverbLv06 reverbLv06 = new ReverbLv06(dataUtilities.convertHexToInt(rdtList.get(6)));
            rdtDataCollectionList.add(reverbLv06);

            //07 Sprite Max in room data
            PriSpriteRoomMax07 priSpriteRoomMax07 = new PriSpriteRoomMax07(dataUtilities.convertHexToInt(rdtList.get(7)));
            rdtDataCollectionList.add(priSpriteRoomMax07);

            //08 snd0.edt address
            Snd0EdtAddress08 snd0EdtAddress08 = new Snd0EdtAddress08(dataUtilities.convertHexToInt(rdtList.get(11) + rdtList.get(10) + rdtList.get(9) + rdtList.get(8)));
            rdtDataCollectionList.add(snd0EdtAddress08);

            //09 snd0.vh address
            Snd0VhAddress09 snd0VhAddress09 = new Snd0VhAddress09(dataUtilities.convertHexToInt(rdtList.get(15) + rdtList.get(14) + rdtList.get(13) + rdtList.get(12)));
            rdtDataCollectionList.add(snd0VhAddress09);

            //10 snd0.vb address
            Snd0VbAddress10 snd0VbAddress10 = new Snd0VbAddress10(dataUtilities.convertHexToInt(rdtList.get(19) + rdtList.get(18) + rdtList.get(17) + rdtList.get(16)));
            rdtDataCollectionList.add(snd0VbAddress10);

            //11 snd1.edt address
            Snd1EdtAddress11 snd1EdtAddress11 = new Snd1EdtAddress11(dataUtilities.convertHexToInt(rdtList.get(23) + rdtList.get(22) + rdtList.get(21) + rdtList.get(20)));
            rdtDataCollectionList.add(snd1EdtAddress11);

            //12 snd1.vh address
            Snd1VhAddress12 snd1VhAddress12 = new Snd1VhAddress12(dataUtilities.convertHexToInt(rdtList.get(27) + rdtList.get(26) + rdtList.get(25) + rdtList.get(24)));
            rdtDataCollectionList.add(snd1VhAddress12);

            //13 snd1.vb address
            Snd1VbAddress13 snd1VbAddress13 = new Snd1VbAddress13(dataUtilities.convertHexToInt(rdtList.get(31) + rdtList.get(30) + rdtList.get(29) + rdtList.get(28)));
            rdtDataCollectionList.add(snd1VbAddress13);

            //14 collision.sca address
            CollisionScaAddress14 collisionScaAddress14 = new CollisionScaAddress14(dataUtilities.convertHexToInt(rdtList.get(35) + rdtList.get(34) + rdtList.get(33) + rdtList.get(32)));
            rdtDataCollectionList.add(collisionScaAddress14);

            //15 camera.rid address
            CameraRidAddress15 cameraRidAddress15 = new CameraRidAddress15(dataUtilities.convertHexToInt(rdtList.get(39) + rdtList.get(38) + rdtList.get(37) + rdtList.get(36)));
            rdtDataCollectionList.add(cameraRidAddress15);

            //16 zone.rvd address
            ZoneRvdAddress16 zoneRvdAddress16 = new ZoneRvdAddress16(dataUtilities.convertHexToInt(rdtList.get(43) + rdtList.get(42) + rdtList.get(41) + rdtList.get(40)));
            rdtDataCollectionList.add(zoneRvdAddress16);

            //17 light.lit address
            LightLitAddress17 lightLitAddress17 = new LightLitAddress17(dataUtilities.convertHexToInt(rdtList.get(47) + rdtList.get(46) + rdtList.get(45) + rdtList.get(44)));
            rdtDataCollectionList.add(lightLitAddress17);

            //18 md1 tim pointer address
            Md1TimPointerAddress18 md1TimPointerAddress18 = new Md1TimPointerAddress18(dataUtilities.convertHexToInt(rdtList.get(51) + rdtList.get(50) + rdtList.get(49) + rdtList.get(48)));
            rdtDataCollectionList.add(md1TimPointerAddress18);

            //19 floor.flr address
            FloorFlrAddress19 floorFlrAddress19 = new FloorFlrAddress19(dataUtilities.convertHexToInt(rdtList.get(55) + rdtList.get(54) + rdtList.get(53) + rdtList.get(52)));
            rdtDataCollectionList.add(floorFlrAddress19);

            //20 block.blk address
            BlockBlkAddress20 blockBlkAddress20 = new BlockBlkAddress20(dataUtilities.convertHexToInt(rdtList.get(59) + rdtList.get(58) + rdtList.get(57) + rdtList.get(56)));
            rdtDataCollectionList.add(blockBlkAddress20);

            //21 main.msg offset address
            MessageMsgAddress21 messageMsgAddress21 = new MessageMsgAddress21(dataUtilities.convertHexToInt(rdtList.get(63) + rdtList.get(62) + rdtList.get(61) + rdtList.get(60)));
            rdtDataCollectionList.add(messageMsgAddress21);

            //22 main.scd offset address
            MainScdOffsetAddress22 mainScdOffsetAddress22 = new MainScdOffsetAddress22(dataUtilities.convertHexToInt(rdtList.get(67) + rdtList.get(66) + rdtList.get(65) + rdtList.get(64)));
            rdtDataCollectionList.add(mainScdOffsetAddress22);

            //23 sub.scd offset address
            SubScdOffsetAddress23 subScdOffsetAddress23 = new SubScdOffsetAddress23(dataUtilities.convertHexToInt(rdtList.get(71) + rdtList.get(70) + rdtList.get(69) + rdtList.get(68)));
            rdtDataCollectionList.add(subScdOffsetAddress23);

            //24 extra.scd offset address
            ExtraScdOffsetAddress24 extraScdOffsetAddress24 = new ExtraScdOffsetAddress24(dataUtilities.convertHexToInt(rdtList.get(75) + rdtList.get(74) + rdtList.get(73) + rdtList.get(72)));
            rdtDataCollectionList.add(extraScdOffsetAddress24);

            //25 effect.esp address
            EffectEspAddress25 effectEspAddress25 = new EffectEspAddress25(dataUtilities.convertHexToInt(rdtList.get(79) + rdtList.get(78) + rdtList.get(77) + rdtList.get(76)));
            rdtDataCollectionList.add(effectEspAddress25);

            //26 something evtl eff address
            SomethingEvtlEffAddress26 somethingEvtlEffAddress26 = new SomethingEvtlEffAddress26(dataUtilities.convertHexToInt(rdtList.get(83) + rdtList.get(82) + rdtList.get(81) + rdtList.get(80)));
            rdtDataCollectionList.add(somethingEvtlEffAddress26);

            //27 esp.tim address
            EspTimAddress27 espTimAddress27 = new EspTimAddress27(dataUtilities.convertHexToInt(rdtList.get(87) + rdtList.get(86) + rdtList.get(85) + rdtList.get(84)));
            rdtDataCollectionList.add(espTimAddress27);

            //28 model.tim texture md1 address
            ModelTimAddress28 modelTimAddress28 = new ModelTimAddress28(dataUtilities.convertHexToInt(rdtList.get(91) + rdtList.get(90) + rdtList.get(89) + rdtList.get(88)));
            rdtDataCollectionList.add(modelTimAddress28);

            //29 animation.rbj address
            AnimationRbjAddress29 animationRbjAddress29 = new AnimationRbjAddress29(dataUtilities.convertHexToInt(rdtList.get(95) + rdtList.get(94) + rdtList.get(93) + rdtList.get(92)));
            rdtDataCollectionList.add(animationRbjAddress29);

            //FETCH THE SPECIAL CASES FIRST, THAT WE DON'T GET WITH AN ADRESS POINTER
            List<Object> timMd1List = new ArrayList<>();

            //Get the Tim/MD1 List Items
            if(md1Room02.getMd1() != 0) {
                if(zoneRvdAddress16.getAddress() != 0) {
                    timMd1List = rdtUtilities.fetchTimAndMd1AddressData(md1TimPointerAddress18.getAddress(), zoneRvdAddress16.getAddress() - 1, rdtList);
                }
                else if(lightLitAddress17.getAddress() != 0) {
                    timMd1List = rdtUtilities.fetchTimAndMd1AddressData(md1TimPointerAddress18.getAddress(), lightLitAddress17.getAddress() - 1, rdtList);
                }
                if(modelTimAddress28.getAddress() == 0) {
                    if(timMd1List.size() > 0) {
                        ModelTimElementAddress31 modelTimElementAddress31 = (ModelTimElementAddress31) timMd1List.get(0);
                        modelTimAddress28.setAddress(modelTimElementAddress31.getAddress());
                    }
                }
            }

            //1. UNKNOWN DATA ADRESS
            UnknownDataAddress61 unknownDataAddress61 = rdtUtilities.fetchUnknownDataAddress(modelTimAddress28, rdtList);

            //2. SND0.SND und SND1.SND ADRESS
            ArrayList<Object> snd0Andsnd1AddressList = rdtUtilities.fetchSnd0AndSnd1Address(espTimAddress27, unknownDataAddress61, modelTimAddress28);

            Snd0SndAddress62 snd0SndAddress62 = (Snd0SndAddress62) snd0Andsnd1AddressList.get(0);
            Snd1SndAddress63 snd1SndAddress63 = (Snd1SndAddress63) snd0Andsnd1AddressList.get(1);

            //Get first element of md1
            int addressOfFirstMd1 = 0;
            if(timMd1List.size() != 0) {
                if (timMd1List.get(1) != null) {
                    ModelMd1ElementAddress32 modelMd1ElementAddress32 = (ModelMd1ElementAddress32) timMd1List.get(1);
                    addressOfFirstMd1 = modelMd1ElementAddress32.getAddress();
                }
            }

            //Create Map and List in Order to find the types that follow.
            classPositionMapInOrder = rdtUtilities.createOrderMap();
            classOffsetStartListInOrder = rdtUtilities.createOrderOffsetList(cameraRidAddress15.getAddress(), md1TimPointerAddress18.getAddress(),
                    zoneRvdAddress16.getAddress(), lightLitAddress17.getAddress(), 0, collisionScaAddress14.getAddress(), blockBlkAddress20.getAddress(),
                    floorFlrAddress19.getAddress(), mainScdOffsetAddress22.getAddress(), subScdOffsetAddress23.getAddress(), extraScdOffsetAddress24.getAddress(),
                    messageMsgAddress21.getAddress(), animationRbjAddress29.getAddress(), addressOfFirstMd1, effectEspAddress25.getAddress(), snd0EdtAddress08.getAddress(),
                    snd0VhAddress09.getAddress(), snd1EdtAddress11.getAddress(), snd1VhAddress12.getAddress(), snd0VbAddress10.getAddress(), snd1VbAddress13.getAddress(),
                    snd0SndAddress62.getAddress(), snd1SndAddress63.getAddress(), espTimAddress27.getAddress(), unknownDataAddress61.getAddress(), modelTimAddress28.getAddress());

            //3. SPRITE.PRI ADDRESS
            SpritePriAddress64 spritePriAddress64;
            if(lightLitAddress17.getAddress() != 0) {
                //Try to get the pri data on the proper way:
                int endOfLightLit = (lightLitAddress17.getAddress() + (cut01.getCut() * 40))-1;
                int startOfPri = endOfLightLit + 1;
                int endOfSpritePri = rdtUtilities.getEndDataOfElement(SpritePriAddress64.class.getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                spritePriAddress64 = new SpritePriAddress64(startOfPri, startOfPri, endOfSpritePri-1);
                classOffsetStartListInOrder.set(classPositionMapInOrder.get(spritePriAddress64.getClass().getCanonicalName()), spritePriAddress64.getRdtOffsetPositionStart());
            }
            else {
                spritePriAddress64 = rdtUtilities.fetchSpritePriAddressAlternatively(classPositionMapInOrder, classOffsetStartListInOrder, rdtList, rdtFilePath);
                classOffsetStartListInOrder.set(classPositionMapInOrder.get(spritePriAddress64.getClass().getCanonicalName()), spritePriAddress64.getRdtOffsetPositionStart());
            }

            //NOW WE START PARING ONE AFTER ANOTHER
            //01 - camera.rid
            int endDataOfElement;
            if(cut01.getCut() != 0 && cameraRidAddress15.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(cameraRidAddress15.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String cameraRidData = rdtUtilities.fetchRdtDataFromOffsets(cameraRidAddress15.getAddress(), endDataOfElement, rdtList);
                CameraRid30 cameraRid30 = new CameraRid30(cameraRidData, cameraRidAddress15.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(cameraRid30);
            }

            //02 - tim/md1 address
            if(timMd1List.size() > 0) {
                rdtDataCollectionList.addAll(timMd1List);
            }

            //03 zone.rvd data
            if(zoneRvdAddress16.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(zoneRvdAddress16.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String zoneRvdData = rdtUtilities.fetchRdtDataFromOffsets(zoneRvdAddress16.getAddress(), endDataOfElement, rdtList);
                ZoneRvd33 zoneRvd33 = new ZoneRvd33(zoneRvdData, zoneRvdAddress16.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(zoneRvd33);
            }

            //04 light.lit data
            if(lightLitAddress17.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(lightLitAddress17.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String lightLitData = rdtUtilities.fetchRdtDataFromOffsets(lightLitAddress17.getAddress(), endDataOfElement, rdtList);
                LightLit34 lightLit34 = new LightLit34(lightLitData, lightLitAddress17.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(lightLit34);
            }

            //05 sprite.pri data
            if(spritePriAddress64.getRdtOffsetPositionStart() != 0) {
                String spritePriData = rdtUtilities.fetchRdtDataFromOffsets(spritePriAddress64.getRdtOffsetPositionStart(), spritePriAddress64.getRdtOffsetPositionEnd() + 1, rdtList);

                SpritePri35 spritePri35 = new SpritePri35(spritePriData, spritePriAddress64.getRdtOffsetPositionStart(), spritePriAddress64.getRdtOffsetPositionEnd());
                rdtDataCollectionList.add(spritePri35);
            }

            //06 collision.sca data
            if(collisionScaAddress14.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(collisionScaAddress14.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String lightLitData = rdtUtilities.fetchRdtDataFromOffsets(collisionScaAddress14.getAddress(), endDataOfElement, rdtList);
                CollisionSca36 collisionSca36 = new CollisionSca36(lightLitData, collisionScaAddress14.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(collisionSca36);
            }

            //07 block.blk data
            if(blockBlkAddress20.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(blockBlkAddress20.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String blockBlkData = rdtUtilities.fetchRdtDataFromOffsets(blockBlkAddress20.getAddress(), endDataOfElement, rdtList);
                BlockBlk37 blockBlk37 = new BlockBlk37(blockBlkData, blockBlkAddress20.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(blockBlk37);
            }

            //08 floor.flr data
            if(floorFlrAddress19.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(floorFlrAddress19.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String floorFlrData = rdtUtilities.fetchRdtDataFromOffsets(floorFlrAddress19.getAddress(), endDataOfElement, rdtList);
                FloorFlr38 floorFlr38 = new FloorFlr38(floorFlrData, floorFlrAddress19.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(floorFlr38);
            }

            //09 offset of main.scd data
            List<Object> mainScdOffsets = new ArrayList<>();
            if(mainScdOffsetAddress22.getAddress() != 0) {
                int mainScdDataStart = rdtUtilities.fetchStartOfDataFromOffset(mainScdOffsetAddress22.getAddress(), rdtList);

                mainScdOffsets = rdtUtilities.fetchOffsets("main", mainScdOffsetAddress22.getAddress(), mainScdDataStart - 1, rdtList);
                rdtDataCollectionList.addAll(mainScdOffsets);
            }

            //10 main.scd data
            if(mainScdOffsetAddress22.getAddress() != 0) {
                List<Object> mainScdDataList = rdtUtilities.fetchData(mainScdOffsetAddress22.getAddress(), subScdOffsetAddress23.getAddress(), rdtList, mainScdOffsets);
                rdtDataCollectionList.addAll(mainScdDataList);
            }

            //11 offset of sub.scd data
            List<Object> subScdOffsets = new ArrayList<>();
            if(subScdOffsetAddress23.getAddress() != 0) {
                int subScdDataStart = rdtUtilities.fetchStartOfDataFromOffset(subScdOffsetAddress23.getAddress(), rdtList);

                subScdOffsets = rdtUtilities.fetchOffsets("sub", subScdOffsetAddress23.getAddress(), subScdDataStart - 1, rdtList);
                rdtDataCollectionList.addAll(subScdOffsets);
            }

            //12 sub.scd data
            if(subScdOffsetAddress23.getAddress() != 0) {
                List<Object> subScdDataList = null;
                ModelMd1ElementAddress32 modelMd1ElementAddress32 = null;
                if(timMd1List.size() > 1) {
                    if(timMd1List.get(1).getClass().getCanonicalName().equals(ModelMd1ElementAddress32.class.getCanonicalName())) {
                        modelMd1ElementAddress32 = (ModelMd1ElementAddress32) timMd1List.get(1);
                    }
                }
                if(extraScdOffsetAddress24.getAddress() != 0) {
                    subScdDataList = rdtUtilities.fetchData(subScdOffsetAddress23.getAddress(), extraScdOffsetAddress24.getAddress(), rdtList, subScdOffsets);
                }
                else if(messageMsgAddress21.getAddress() != 0) {
                    subScdDataList = rdtUtilities.fetchData(subScdOffsetAddress23.getAddress(), messageMsgAddress21.getAddress(), rdtList, subScdOffsets);
                }
                else if(animationRbjAddress29.getAddress() != 0) {
                    subScdDataList = rdtUtilities.fetchData(subScdOffsetAddress23.getAddress(), animationRbjAddress29.getAddress(), rdtList, subScdOffsets);
                }
                else  if(modelMd1ElementAddress32 != null) {
                    if(modelMd1ElementAddress32.getAddress() != 0) {
                        subScdDataList = rdtUtilities.fetchData(subScdOffsetAddress23.getAddress(), modelMd1ElementAddress32.getAddress(), rdtList, subScdOffsets);
                    }
                }
                else if(effectEspAddress25.getAddress() != 0) {
                    subScdDataList = rdtUtilities.fetchData(subScdOffsetAddress23.getAddress(), effectEspAddress25.getAddress(), rdtList, subScdOffsets);
                }
                else if(snd0EdtAddress08.getAddress() != 0) {
                    subScdDataList = rdtUtilities.fetchData(subScdOffsetAddress23.getAddress(), snd0EdtAddress08.getAddress(), rdtList, subScdOffsets);
                }

                rdtDataCollectionList.addAll(subScdDataList);
            }

            //13 offset of extra.scd data
            List<Object> extraScdOffsets = new ArrayList<>();
            if(extraScdOffsetAddress24.getAddress() != 0) {
                int extraScdDataStart = rdtUtilities.fetchStartOfDataFromOffset(extraScdOffsetAddress24.getAddress(), rdtList);

                extraScdOffsets = rdtUtilities.fetchOffsets("extra", extraScdOffsetAddress24.getAddress(), extraScdDataStart - 1, rdtList);
                rdtDataCollectionList.addAll(extraScdOffsets);
            }

            //14 extra.scd data
            if(extraScdOffsetAddress24.getAddress() != 0) {
                List<Object> extraScdDataList = null;
                ModelMd1ElementAddress32 modelMd1ElementAddress32 = null;
                if(timMd1List.size() > 1) {
                    if(timMd1List.get(1).getClass().getCanonicalName().equals(ModelMd1ElementAddress32.class.getCanonicalName())) {
                        modelMd1ElementAddress32 = (ModelMd1ElementAddress32) timMd1List.get(1);
                    }
                }
                if(messageMsgAddress21.getAddress() != 0) {
                    extraScdDataList = rdtUtilities.fetchData(extraScdOffsetAddress24.getAddress(), messageMsgAddress21.getAddress(), rdtList, extraScdOffsets);
                }
                else if(animationRbjAddress29.getAddress() != 0) {
                    extraScdDataList = rdtUtilities.fetchData(extraScdOffsetAddress24.getAddress(), animationRbjAddress29.getAddress(), rdtList, extraScdOffsets);
                }
                else  if(modelMd1ElementAddress32 != null) {
                    if(modelMd1ElementAddress32.getAddress() != 0) {
                        extraScdDataList = rdtUtilities.fetchData(extraScdOffsetAddress24.getAddress(), modelMd1ElementAddress32.getAddress(), rdtList, extraScdOffsets);
                    }
                }
                else if(effectEspAddress25.getAddress() != 0) {
                    extraScdDataList = rdtUtilities.fetchData(extraScdOffsetAddress24.getAddress(), effectEspAddress25.getAddress(), rdtList, extraScdOffsets);
                }
                else if(snd0EdtAddress08.getAddress() != 0) {
                    extraScdDataList = rdtUtilities.fetchData(extraScdOffsetAddress24.getAddress(), snd0EdtAddress08.getAddress(), rdtList, extraScdOffsets);
                }
                rdtDataCollectionList.addAll(extraScdDataList);
            }

            //15 offset of main.msg data
            List<Object> mainMsgOffsets = new ArrayList<>();
            if(messageMsgAddress21.getAddress() != 0) {
                int mainMsgDataStart = rdtUtilities.fetchStartOfDataFromOffset(messageMsgAddress21.getAddress(), rdtList);

                //Fetch the single mainScdOffset elements
                mainMsgOffsets = rdtUtilities.fetchOffsets("msg", messageMsgAddress21.getAddress(), mainMsgDataStart - 1, rdtList);
                rdtDataCollectionList.addAll(mainMsgOffsets);
            }

            //16 main.msg data
            if(messageMsgAddress21.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(messageMsgAddress21.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                List<Object> mainMsgDataList = rdtUtilities.fetchData(messageMsgAddress21.getAddress(), endDataOfElement, rdtList, mainMsgOffsets);
                rdtDataCollectionList.addAll(mainMsgDataList);
            }

            //17 animation.rbj data
            if(animationRbjAddress29.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(animationRbjAddress29.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String animationRbjData = rdtUtilities.fetchRdtDataFromOffsets(animationRbjAddress29.getAddress(), endDataOfElement, rdtList);
                AnimationRbj47 animationRbj47 = new AnimationRbj47(animationRbjData, animationRbjAddress29.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(animationRbj47);
            }

            //18 modelXX.md1 data
            if(timMd1List.size() > 1) {
                if(timMd1List.get(1).getClass().getCanonicalName().equals(ModelMd1ElementAddress32.class.getCanonicalName())) {
                    rdtUtilities.fetchMd1ElementsAndSaveToResultList(classPositionMapInOrder, classOffsetStartListInOrder, timMd1List, rdtList, rdtDataCollectionList);
                }
            }

            //19 effect.esp data
            if(effectEspAddress25.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(effectEspAddress25.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String effectEspData = rdtUtilities.fetchRdtDataFromOffsets(effectEspAddress25.getAddress(), endDataOfElement, rdtList);
                EffectEsp58 effectEsp58 = new EffectEsp58(effectEspData, effectEspAddress25.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(effectEsp58);
            }

            //20 snd0.edt data
            if(snd0EdtAddress08.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(snd0EdtAddress08.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String snd0EdtData = rdtUtilities.fetchRdtDataFromOffsets(snd0EdtAddress08.getAddress(), endDataOfElement, rdtList);
                Snd0Edt49 snd0Edt49 = new Snd0Edt49(snd0EdtData, snd0EdtAddress08.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(snd0Edt49);
            }

            //21 snd0.vh data
            if(snd0VhAddress09.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(snd0VhAddress09.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String snd0VhData = rdtUtilities.fetchRdtDataFromOffsets(snd0VhAddress09.getAddress(), endDataOfElement, rdtList);
                Snd0Vh50 snd0Vh50 = new Snd0Vh50(snd0VhData, snd0VhAddress09.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(snd0Vh50);
            }

            //22 snd1.edt data
            if(snd1EdtAddress11.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(snd1EdtAddress11.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String snd1EdtData = rdtUtilities.fetchRdtDataFromOffsets(snd1EdtAddress11.getAddress(), endDataOfElement, rdtList);
                Snd1Edt51 snd1Edt51 = new Snd1Edt51(snd1EdtData, snd1EdtAddress11.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(snd1Edt51);
            }

            //23 snd1.vh data
            if(snd1VhAddress12.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(snd1VhAddress12.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String snd1VhData = rdtUtilities.fetchRdtDataFromOffsets(snd1VhAddress12.getAddress(), endDataOfElement, rdtList);
                Snd1Vh52 snd1Vh52 = new Snd1Vh52(snd1VhData, snd1VhAddress12.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(snd1Vh52);
            }

            //24 snd0.vb data
            if(snd0VbAddress10.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(snd0VbAddress10.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String snd0VbData = rdtUtilities.fetchRdtDataFromOffsets(snd0VbAddress10.getAddress(), endDataOfElement, rdtList);
                Snd0Vb53 snd0Vb53 = new Snd0Vb53(snd0VbData, snd0VbAddress10.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(snd0Vb53);
            }

            //25 snd1.vb data
            if(snd1VbAddress13.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(snd1VbAddress13.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String snd1VbData = rdtUtilities.fetchRdtDataFromOffsets(snd1VbAddress13.getAddress(), endDataOfElement, rdtList);
                Snd1Vb54 snd1Vb54 = new Snd1Vb54(snd1VbData, snd1VbAddress13.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(snd1Vb54);
            }

            //26 snd0.snd data
            if(snd0SndAddress62.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(snd0SndAddress62.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String snd0SndData = rdtUtilities.fetchRdtDataFromOffsets(snd0SndAddress62.getAddress(), endDataOfElement, rdtList);
                Snd0Snd55 snd0Snd55 = new Snd0Snd55(snd0SndData, snd0SndAddress62.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(snd0Snd55);
            }

            //27 snd1.snd data
            if(snd1SndAddress63.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(snd1SndAddress63.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String snd1SndData = rdtUtilities.fetchRdtDataFromOffsets(snd1SndAddress63.getAddress(), endDataOfElement, rdtList);
                Snd1Snd56 snd1Snd56 = new Snd1Snd56(snd1SndData, snd1SndAddress63.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(snd1Snd56);
            }

            //28 esp.tim data
            if(espTimAddress27.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(espTimAddress27.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                List<Object> espTimAddressList = rdtUtilities.fetchEspTimAddress(espTimAddress27.getAddress(), endDataOfElement, rdtList);
                for(int i = 0; i < espTimAddressList.size(); i++) {
                    EspTimElementAddress65 espTimElementAddress65 = (EspTimElementAddress65) espTimAddressList.get(i);
                    String espTimData = rdtUtilities.fetchRdtDataFromOffsets(espTimElementAddress65.getRdtOffsetPositionStart(), espTimElementAddress65.getRdtOffsetPositionEnd() + 1, rdtList);
                    EspTim59 espTim59 = new EspTim59(espTimData, espTimElementAddress65.getRdtOffsetPositionStart(), espTimElementAddress65.getRdtOffsetPositionEnd());
                    rdtDataCollectionList.add(espTim59);
                }
            }

            //29 unknown data
            if(unknownDataAddress61.getAddress() != 0) {
                endDataOfElement = rdtUtilities.getEndDataOfElement(unknownDataAddress61.getClass().getCanonicalName(), classPositionMapInOrder, classOffsetStartListInOrder);
                String unknownData = rdtUtilities.fetchRdtDataFromOffsets(unknownDataAddress61.getAddress(), endDataOfElement, rdtList);
                UnknownData60 unknownData60 = new UnknownData60(unknownData, unknownDataAddress61.getAddress(), endDataOfElement - 1);
                rdtDataCollectionList.add(unknownData60);
            }

            //30 modelXX.tim data
            if(timMd1List.size() > 0) {
                if(timMd1List.get(0).getClass().getCanonicalName().equals(ModelTimElementAddress31.class.getCanonicalName())) {
                    rdtUtilities.fetchTimElementsAndSaveToResultList(classPositionMapInOrder, classOffsetStartListInOrder, timMd1List, rdtList, rdtDataCollectionList);
                }
            }
        }

        return rdtDataCollectionList;
    }

    public void parseScdElements() {
        mainScdDataElements = rdtUtilities.fetchMainScdDataElements(rdtObjectList);
        subScdDataElements = rdtUtilities.fetchSubScdDataElements(rdtObjectList);
        extraScdDataElements = rdtUtilities.fetchExtraScdDataElements(rdtObjectList);
        allScdDataElements = new ArrayList<>();
        allScdDataElements.addAll(mainScdDataElements);
        allScdDataElements.addAll(subScdDataElements);
        allScdDataElements.addAll(extraScdDataElements);

        //Parse scd element data
        rdtUtilities.parseScdElements(mainScdDataElements);
        rdtUtilities.parseScdElements(subScdDataElements);
        rdtUtilities.parseScdElements(extraScdDataElements);
    }

    public void setCurrentProcessedBackgroundImage(File[] currentProcessedBackgroundImage) {
        this.currentProcessedBackgroundImage = currentProcessedBackgroundImage;
    }

    public void setNewRdtList(ArrayList<String> newRdtList) {
        this.rdtList = newRdtList;
    }

    public void clearImageInstance() {
        INSTANCE = null;
    }
}
