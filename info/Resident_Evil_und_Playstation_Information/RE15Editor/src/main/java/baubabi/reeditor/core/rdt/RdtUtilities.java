package baubabi.reeditor.core.rdt;

import baubabi.reeditor.core.general.utils.DataUtilities;
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
import baubabi.reeditor.core.rdt._25_effect_esp_address.EffectEspAddress25;
import baubabi.reeditor.core.rdt._27_esp_tim_address.EspTimAddress27;
import baubabi.reeditor.core.rdt._28_model_tim_address.ModelTimAddress28;
import baubabi.reeditor.core.rdt._29_animation_rbj_address.AnimationRbjAddress29;
import baubabi.reeditor.core.rdt._31_model_tim_element_address.ModelTimElementAddress31;
import baubabi.reeditor.core.rdt._32_model_md1_element_address.ModelMd1ElementAddress32;
import baubabi.reeditor.core.rdt._39_main_scd_offset.MainScdOffset39;
import baubabi.reeditor.core.rdt._40_main_scd.MainScd40;
import baubabi.reeditor.core.rdt._41_sub_scd_offset.SubScdOffset41;
import baubabi.reeditor.core.rdt._42_sub_scd.SubScd42;
import baubabi.reeditor.core.rdt._43_extra_scd_offset.ExtraScdOffset43;
import baubabi.reeditor.core.rdt._44_extra_scd.ExtraScd44;
import baubabi.reeditor.core.rdt._45_main_msg_offset.MainMsgOffset45;
import baubabi.reeditor.core.rdt._46_main_msg.MainMsg46;
import baubabi.reeditor.core.rdt._48_model_md1.ModelMd148;
import baubabi.reeditor.core.rdt._57_model_tim.ModelTim57;
import baubabi.reeditor.core.rdt._61_unknown_data_address.UnknownDataAddress61;
import baubabi.reeditor.core.rdt._62_snd0_snd_address.Snd0SndAddress62;
import baubabi.reeditor.core.rdt._63_snd1_snd_address.Snd1SndAddress63;
import baubabi.reeditor.core.rdt._64_sprite_pri_address.SpritePriAddress64;
import baubabi.reeditor.core.rdt._65_esp_tim_element_address.EspTimElementAddress65;
import baubabi.reeditor.core.rdt.scd.ScdUtilities;
import lombok.extern.log4j.Log4j2;
import org.apache.commons.lang3.StringUtils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

@Log4j2
public class RdtUtilities {
    private static RdtUtilities INSTANCE;
    private final DataUtilities dataUtilities;

    private RdtUtilities() {
        dataUtilities = DataUtilities.getInstance();
    }

    public static RdtUtilities getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new RdtUtilities();
        }

        return INSTANCE;
    }

    public String fetchRdtDataFromOffsets(int startOffset, int endOffset, List<String> rdtList) {
        String dataFromRdt = "";
        List<String> newStringList;
        newStringList = rdtList.subList(startOffset, endOffset);
        dataFromRdt = StringUtils.join(newStringList, "");

        return dataFromRdt;
    }

    public int figureOutIfUnknownDataExist(int startOffset, int endOffset, List<String> rdtList) {
        int numOfHits = endOffset - startOffset;
        int counter = 0;
        int unknownDataStartOffset = 0;
        for (int i = startOffset; i < endOffset; i++) {
            if (rdtList.get(i).equals("FF")) {
                counter = counter + 1;
            }
        }
        if (counter == numOfHits) {
            //PATTERN FIT, WE HAVE MULTIPLE FF, we have unknown data
            unknownDataStartOffset = startOffset;
        }
        return unknownDataStartOffset;
    }

    public List<Object> fetchTimAndMd1AddressData(int startOffset, int endOffset, List<String> rdtList) {
        List<Object> timMd1AddressList = new ArrayList<>();
        boolean timStart = true;

        for (int i = startOffset; i < endOffset; i = i + 4) {

            if (timStart) {
                ModelTimElementAddress31 modelTimElementAddress31 = new ModelTimElementAddress31(dataUtilities.convertHexToInt(rdtList.get(i + 3) + rdtList.get(i + 2) + rdtList.get(i + 1) + rdtList.get(i)), i, i + 3);
                timMd1AddressList.add(modelTimElementAddress31);
                timStart = false;
            } else {
                ModelMd1ElementAddress32 modelMd1ElementAddress32 = new ModelMd1ElementAddress32(dataUtilities.convertHexToInt(rdtList.get(i + 3) + rdtList.get(i + 2) + rdtList.get(i + 1) + rdtList.get(i)), i, i + 3);
                timMd1AddressList.add(modelMd1ElementAddress32);
                timStart = true;
            }

        }

        return timMd1AddressList;
    }

    public List<Object> fetchEspTimAddress(int startOffset, int endOffset, List<String> rdtList) {
        List<Object> espTimAddressList = new ArrayList<>();
        int currentStartOffset = startOffset;

        for (int i = startOffset + 1; i < endOffset; i++) {
            //As far as figured out, the esp.tim data always starts with
            //10000000080000002C0000000000E001100001000000 and then changes afterwards depending on data
            if (endOffset > i + 18) {
                if (rdtList.get(i).equals("10")
                        && rdtList.get(i + 1).equals("00")
                        && rdtList.get(i + 2).equals("00")
                        && rdtList.get(i + 3).equals("00")
                        && rdtList.get(i + 4).equals("08")
                        && rdtList.get(i + 5).equals("00")
                        && rdtList.get(i + 6).equals("00")
                        && rdtList.get(i + 7).equals("00")
                        && rdtList.get(i + 8).equals("2C")
                        && rdtList.get(i + 9).equals("00")
                        && rdtList.get(i + 10).equals("00")
                        && rdtList.get(i + 11).equals("00")
                        && rdtList.get(i + 12).equals("00")
                        && rdtList.get(i + 13).equals("00")
                        && rdtList.get(i + 14).equals("E0")
                        && rdtList.get(i + 15).equals("01")
                        && rdtList.get(i + 16).equals("10")
                        && rdtList.get(i + 17).equals("00")
                        && rdtList.get(i + 18).equals("01")) {
                    EspTimElementAddress65 espTimElementAddress65 = new EspTimElementAddress65(currentStartOffset, currentStartOffset, i-1);
                    espTimAddressList.add(espTimElementAddress65);
                    currentStartOffset = i;
                }
            }
        }

        EspTimElementAddress65 espTimElementAddress65 = new EspTimElementAddress65(currentStartOffset, currentStartOffset, endOffset-1);
        espTimAddressList.add(espTimElementAddress65);

        return espTimAddressList;
    }

    public int figureOutWhereSpritePriStarts(int startOffset, int latestEnd, List<String> rdtList, String rdtFilePath) {
        int startAddressOfSpritePri = 0;

        for (int i = startOffset; i < latestEnd; i = i + 40) {
            if (rdtList.size() > i + 1) {
                if (rdtList.get(i).equals("FF")) {
                    if (rdtList.size() > i + 1) {
                        if (rdtList.get(i + 1).equals("FF")) {
                            startAddressOfSpritePri = i;
                            break;
                        }
                    }
                }
            }
        }
        if (startAddressOfSpritePri == 0) {
            log.error("Couldn't find the Start Address of sprite.pri for " + rdtFilePath);
        }
        return startAddressOfSpritePri;
    }

    public int getEndDataOfElement(String classObject, HashMap<String, Integer> mapOfElementsInOrder, ArrayList<Integer> listOfElementOffsetsInOrder) {
        int dataElementEnd = 0;
        int elementStartIndex = mapOfElementsInOrder.get(classObject);

        //Get the End that is not 0
        for (int i = elementStartIndex + 1; i < listOfElementOffsetsInOrder.size(); i++) {
            int offsetOfElementAfter = listOfElementOffsetsInOrder.get(i);
            if (offsetOfElementAfter != 0) {
                dataElementEnd = offsetOfElementAfter;
                break;
            }
        }
        if(dataElementEnd == 0) {
            dataElementEnd = RdtProcessing.getInstance().getRdtList().size();
        }

        return dataElementEnd;
    }

    public UnknownDataAddress61 fetchUnknownDataAddress(ModelTimAddress28 modelTimAddress28, List<String> rdtList) {
        int startOfUnknownData;
        UnknownDataAddress61 unknownDataAddress61;

        if(modelTimAddress28.getAddress() != 0) {
            //UNKNOWN DATA START WITH A LOT OF FF. WE CHECK IF WE FIND 5 F - like FFFFFFFFFF before Tim Data
            startOfUnknownData = figureOutIfUnknownDataExist(modelTimAddress28.getAddress() - 32, modelTimAddress28.getAddress() - 27, rdtList);
            unknownDataAddress61 = new UnknownDataAddress61(startOfUnknownData, startOfUnknownData, startOfUnknownData + 31);
        }
        else {
            //UNKNOWN DATA START WITH A LOT OF FF. WE CHECK IF WE FIND 5 F - like FFFFFFFFFF before List end
            startOfUnknownData = figureOutIfUnknownDataExist(rdtList.size() - 32, rdtList.size() - 27, rdtList);
            unknownDataAddress61 = new UnknownDataAddress61(startOfUnknownData, startOfUnknownData, startOfUnknownData + 31);
        }

        return unknownDataAddress61;
    }

    public ArrayList<Object> fetchSnd0AndSnd1Address(EspTimAddress27 espTimAddress27, UnknownDataAddress61 unknownDataAddress61, ModelTimAddress28 modelTimAddress28) {
        ArrayList<Object> resultList = new ArrayList<>();

        int startAddressOfSnd0Snd;
        int startAddressOfSnd1Snd;
        Snd0SndAddress62 snd0SndAddress62;
        Snd1SndAddress63 snd1SndAddress63;

        if(espTimAddress27.getAddress() != 0) {
            //Then we subtract 12 byte from it, so we have the start Address of snd1.snd
            startAddressOfSnd1Snd = espTimAddress27.getAddress() - 12;

            //Then we subtract 12 byte from it, so we have the start Address of snd0.snd
            startAddressOfSnd0Snd = startAddressOfSnd1Snd - 12;

            snd0SndAddress62 = new Snd0SndAddress62(startAddressOfSnd0Snd, startAddressOfSnd0Snd, startAddressOfSnd0Snd + 11);
            snd1SndAddress63 = new Snd1SndAddress63(startAddressOfSnd1Snd, startAddressOfSnd1Snd, startAddressOfSnd1Snd + 11);
            resultList.add(snd0SndAddress62);
            resultList.add(snd1SndAddress63);
        }
        else if(unknownDataAddress61.getAddress() != 0) {
            //Then we subtract 12 byte from it, so we have the start Address of snd1.snd
            startAddressOfSnd1Snd = unknownDataAddress61.getAddress() - 12;

            //Then we subtract 12 byte from it, so we have the start Address of snd0.snd
            startAddressOfSnd0Snd = startAddressOfSnd1Snd - 12;

            snd0SndAddress62 = new Snd0SndAddress62(startAddressOfSnd0Snd, startAddressOfSnd0Snd, startAddressOfSnd0Snd + 11);
            snd1SndAddress63 = new Snd1SndAddress63(startAddressOfSnd1Snd, startAddressOfSnd1Snd, startAddressOfSnd1Snd + 11);
            resultList.add(snd0SndAddress62);
            resultList.add(snd1SndAddress63);
        }
        else if(modelTimAddress28.getAddress() != 0) {
            //Then we subtract 12 byte from it, so we have the start Address of snd1.snd
            startAddressOfSnd1Snd = modelTimAddress28.getAddress() - 12;

            //Then we subtract 12 byte from it, so we have the start Address of snd0.snd
            startAddressOfSnd0Snd = startAddressOfSnd1Snd - 12;

            snd0SndAddress62 = new Snd0SndAddress62(startAddressOfSnd0Snd, startAddressOfSnd0Snd, startAddressOfSnd0Snd + 11);
            snd1SndAddress63 = new Snd1SndAddress63(startAddressOfSnd1Snd, startAddressOfSnd1Snd, startAddressOfSnd1Snd + 11);
            resultList.add(snd0SndAddress62);
            resultList.add(snd1SndAddress63);
        }
        else {
            snd0SndAddress62 = new Snd0SndAddress62(0, 0, 0);
            snd1SndAddress63 = new Snd1SndAddress63(0, 0, 0);
            resultList.add(snd0SndAddress62);
            resultList.add(snd1SndAddress63);
        }
        return resultList;
    }

    public void fetchMd1ElementsAndSaveToResultList(HashMap<String, Integer> mapOfElementsInOrder, ArrayList<Integer> listOfElementOffsetsInOrder, List<Object> timMd1List, List<String> rdtList, List<Object> rdtDataCollectionList) {
        int endDataOfMd1Elements = getEndDataOfElement(ModelMd1ElementAddress32.class.getCanonicalName(), mapOfElementsInOrder, listOfElementOffsetsInOrder);

        //When we find out, that the neighboor element has the same data as the first element, it have to be skipped in the loop
        //And the neighboor Element is the current Element + 2, because the timMd1List is structured as follows:
        //tim offset, md1 offset, tim offset, md1 offset, tim offset ...
        ArrayList<Integer> elementsToSkip = new ArrayList<>();
        for (int i = 1; i < timMd1List.size(); i = i + 2) {

            //Check if next the neighboor (following md1 element) has same address data as the current one. If so, it will be skipped.
            int loopIndex = i;
            AtomicBoolean isDuplicate = new AtomicBoolean(false);
            elementsToSkip.forEach((elementIndexToSkip) -> {
                if (loopIndex == elementIndexToSkip) {
                    isDuplicate.set(true);
                }
            });
            if (isDuplicate.get()) {
                continue;
            }

            ModelMd1ElementAddress32 modelMd1ElementAddress32 = (ModelMd1ElementAddress32) timMd1List.get(i);
            ModelMd148 modelMd148;

            int neighboorElement = i;
            int modelDataEnd = 0;

            ModelMd1ElementAddress32 modelMd1ElementAddress32NextElement;

            //neighboorElement + 2, because the timMd1List always changes between ModelTimAddress31 and ModelMd1Address32 every 2 values see above
            if (timMd1List.size() > neighboorElement + 2) {
                for (int j = i; j < timMd1List.size(); j++) {
                    neighboorElement = neighboorElement + 2;
                    if (timMd1List.size() > neighboorElement) {
                        modelMd1ElementAddress32NextElement = (ModelMd1ElementAddress32) timMd1List.get(neighboorElement);
                        if (modelMd1ElementAddress32.getAddress() == modelMd1ElementAddress32NextElement.getAddress()) {
                            elementsToSkip.add(neighboorElement); //The index of this element can be skipped in the loop
                        } else {
                            //Our next end data for the current md1 element
                            modelDataEnd = modelMd1ElementAddress32NextElement.getAddress() - 1;
                            break;
                        }
                    } else {
                        //We are at the end of the tim/md1 address list, and need the start of the next element afterwards effect.esp or snd0.edt or other
                        modelDataEnd = endDataOfMd1Elements - 1;
                        break;
                    }
                }
            } else {
                modelDataEnd = endDataOfMd1Elements - 1;
            }
            String modelMd1Data = fetchRdtDataFromOffsets(modelMd1ElementAddress32.getAddress(), modelDataEnd +1, rdtList);
            modelMd148 = new ModelMd148(modelMd1Data, modelMd1ElementAddress32.getAddress(), modelDataEnd);
            rdtDataCollectionList.add(modelMd148);
        }
    }

    public void fetchTimElementsAndSaveToResultList(HashMap<String, Integer> mapOfElementsInOrder, ArrayList<Integer> listOfElementOffsetsInOrder, List<Object> timMd1List, List<String> rdtList, List<Object> rdtDataCollectionList) {

        //When we find out, that the neighboor element has the same data as the first element, it have to be skipped in the loop
        //And the neighboor Element is the current Element + 2, because the timMd1List is structured as follows:
        //tim offset, md1 offset, tim offset, md1 offset, tim offset ...
        ArrayList<Integer> elementsToSkip = new ArrayList<>(); //When we find out, that the neighboor element has the same data as the first element, it have to be skipped in the loop
        for (int i = 0; i < timMd1List.size(); i = i + 2) {
            int loopIndex = i;
            AtomicBoolean isDuplicate = new AtomicBoolean(false);
            elementsToSkip.forEach((elementIndexToSkip) -> {
                if (loopIndex == elementIndexToSkip) {
                    isDuplicate.set(true);
                }
            });
            if (isDuplicate.get()) {
                continue;
            }

            ModelTimElementAddress31 modelTimElementAddress31 = (ModelTimElementAddress31) timMd1List.get(i);
            ModelTim57 modelTim57;

            int neighboorElement = i;
            int modelDataEnd = 0;

            ModelTimElementAddress31 modelTimElementAddress31NextElement;

            //neighboorElement + 2, because the timMd1List always changes between ModelTimAddress31 and ModelMd1Address32 every 2 values see above
            if (timMd1List.size() > neighboorElement + 2) {
                for (int j = i; j < timMd1List.size(); j++) {
                    neighboorElement = neighboorElement + 2;
                    if (timMd1List.size() > neighboorElement) {
                        modelTimElementAddress31NextElement = (ModelTimElementAddress31) timMd1List.get(neighboorElement);
                        if (modelTimElementAddress31.getAddress() == modelTimElementAddress31NextElement.getAddress()) {
                            elementsToSkip.add(neighboorElement); //The index of this element can be skipped in the loop
                        } else {
                            //Our next end data for the current md1 element
                            modelDataEnd = modelTimElementAddress31NextElement.getAddress() - 1;
                            break;
                        }
                    } else {
                        //We are at the end of the tim/md1 address list, and need the start of the next element afterwards effect.esp or snd0.edt or other
                        modelDataEnd = rdtList.size() - 1;
                        break;
                    }
                }
            } else {
                modelDataEnd = rdtList.size() - 1;
            }
            String modelTimData = fetchRdtDataFromOffsets(modelTimElementAddress31.getAddress(), modelDataEnd +1, rdtList);
            modelTim57 = new ModelTim57(modelTimData, modelTimElementAddress31.getAddress(), modelDataEnd);
            rdtDataCollectionList.add(modelTim57);
        }
    }

    public HashMap<String, Integer> createOrderMap() {
        //Prepare Fetching with a order List
        HashMap<String, Integer> classPositionHashMap = new HashMap<>();
        classPositionHashMap.put(CameraRidAddress15.class.getCanonicalName(), 0);
        classPositionHashMap.put(Md1TimPointerAddress18.class.getCanonicalName(), 1);
        classPositionHashMap.put(ZoneRvdAddress16.class.getCanonicalName(), 2);
        classPositionHashMap.put(LightLitAddress17.class.getCanonicalName(), 3);
        classPositionHashMap.put(SpritePriAddress64.class.getCanonicalName(), 4);
        classPositionHashMap.put(CollisionScaAddress14.class.getCanonicalName(), 5);
        classPositionHashMap.put(BlockBlkAddress20.class.getCanonicalName(), 6);
        classPositionHashMap.put(FloorFlrAddress19.class.getCanonicalName(), 7);
        classPositionHashMap.put(MainScdOffsetAddress22.class.getCanonicalName(), 8);
        classPositionHashMap.put(SubScdOffsetAddress23.class.getCanonicalName(), 9);
        classPositionHashMap.put(ExtraScdOffset43.class.getCanonicalName(), 10);
        classPositionHashMap.put(MessageMsgAddress21.class.getCanonicalName(), 11);
        classPositionHashMap.put(AnimationRbjAddress29.class.getCanonicalName(), 12);
        classPositionHashMap.put(ModelMd1ElementAddress32.class.getCanonicalName(), 13);
        classPositionHashMap.put(EffectEspAddress25.class.getCanonicalName(), 14);
        classPositionHashMap.put(Snd0EdtAddress08.class.getCanonicalName(), 15);
        classPositionHashMap.put(Snd0VhAddress09.class.getCanonicalName(), 16);
        classPositionHashMap.put(Snd1EdtAddress11.class.getCanonicalName(), 17);
        classPositionHashMap.put(Snd1VhAddress12.class.getCanonicalName(), 18);
        classPositionHashMap.put(Snd0VbAddress10.class.getCanonicalName(), 19);
        classPositionHashMap.put(Snd1VbAddress13.class.getCanonicalName(), 20);
        classPositionHashMap.put(Snd0SndAddress62.class.getCanonicalName(), 21);
        classPositionHashMap.put(Snd1SndAddress63.class.getCanonicalName(), 22);
        classPositionHashMap.put(EspTimAddress27.class.getCanonicalName(), 23);
        classPositionHashMap.put(UnknownDataAddress61.class.getCanonicalName(), 24);
        classPositionHashMap.put(ModelTimAddress28.class.getCanonicalName(), 25);

        return classPositionHashMap;
    }

    public ArrayList<Integer> createOrderOffsetList(int cameraRidAddress15, int md1TimPointerAddress18, int zoneRvdAddress16, int lightLitAddress17, int spritePriAddress, int collisionScaAddress14,
                                      int blockBlkAddress20, int floorFlrAddress19, int mainScdOffsetAddress22, int subScdOffsetAddress23, int extraScdOffsetAddress24,
                                      int messageMsgAddress21, int animationRbjAddress29, int firstMd1Address, int effectEspAddress25, int snd0EdtAddress08,
                                      int snd0VhAddress09, int snd1EdtAddress11, int snd1VhAddress12, int snd0VbAddress10, int snd1VbAddress13, int snd0SndAddress62,
                                      int snd1SndAddress63, int espTimAddress27, int unknownDataAddress61, int modelTimAddress28) {

        ArrayList<Integer> startOffsetsOfData = new ArrayList<>();
        startOffsetsOfData.add(cameraRidAddress15);
        startOffsetsOfData.add(md1TimPointerAddress18);
        startOffsetsOfData.add(zoneRvdAddress16);
        startOffsetsOfData.add(lightLitAddress17);
        startOffsetsOfData.add(spritePriAddress); // spritePriAddress64 wird am Anfang noch nicht gesetzt
        startOffsetsOfData.add(collisionScaAddress14);
        startOffsetsOfData.add(blockBlkAddress20);
        startOffsetsOfData.add(floorFlrAddress19);
        startOffsetsOfData.add(mainScdOffsetAddress22);
        startOffsetsOfData.add(subScdOffsetAddress23);
        startOffsetsOfData.add(extraScdOffsetAddress24);
        startOffsetsOfData.add(messageMsgAddress21);
        startOffsetsOfData.add(animationRbjAddress29);
        startOffsetsOfData.add(firstMd1Address);
        startOffsetsOfData.add(effectEspAddress25);
        startOffsetsOfData.add(snd0EdtAddress08);
        startOffsetsOfData.add(snd0VhAddress09);
        startOffsetsOfData.add(snd1EdtAddress11);
        startOffsetsOfData.add(snd1VhAddress12);
        startOffsetsOfData.add(snd0VbAddress10);
        startOffsetsOfData.add(snd1VbAddress13);
        startOffsetsOfData.add(snd0SndAddress62);
        startOffsetsOfData.add(snd1SndAddress63);
        startOffsetsOfData.add(espTimAddress27);
        startOffsetsOfData.add(unknownDataAddress61);
        startOffsetsOfData.add(modelTimAddress28);

        return startOffsetsOfData;
    }

    public SpritePriAddress64 fetchSpritePriAddressAlternatively(HashMap<String, Integer> classPositionHashMap, ArrayList<Integer> startOffsetsOfData, List<String> rdtList, String rdtFilePath) {
        int elementStart = 0;
        int elementEnd = 0;
        int elementStartIndex = classPositionHashMap.get(SpritePriAddress64.class.getCanonicalName());

        //Get the Start that is not 0
        for(int i = elementStartIndex-1; i >= 0; i--) {
            int offsetOfElementBefore = startOffsetsOfData.get(i);
            if(offsetOfElementBefore != 0) {
                elementStart = offsetOfElementBefore;
                break;
            }
        }

        //Get the End that is not 0
        for(int i = elementStartIndex+1; i < startOffsetsOfData.size(); i++) {
            int offsetOfElementAfter = startOffsetsOfData.get(i);
            if(offsetOfElementAfter != 0) {
                elementEnd = offsetOfElementAfter;
                break;
            }
        }

        int priStart = figureOutWhereSpritePriStarts(elementStart, elementEnd, rdtList, rdtFilePath);

        return new SpritePriAddress64(priStart, priStart, elementEnd-1);
    }

    public int fetchStartOfDataFromOffset(int startOffset, List<String> rdtList) {
        //The first address Parameter describes the beginning of the scd address.
        String startOffsetOfData = rdtList.get(startOffset + 1) + rdtList.get(startOffset);
        int dataStart = dataUtilities.convertHexToInt(startOffsetOfData);
        dataStart = startOffset + dataStart;

        return dataStart;
    }

    public List<Object> fetchOffsets(String offsetClass, int startOffset, int endOffset, List<String> rdtList) {
        List<Object> offsetList = new ArrayList<>();
        for (int i = startOffset; i < endOffset; i = i + 2) {
            int offset = dataUtilities.convertHexToInt(rdtList.get(i + 1) + rdtList.get(i));
            if (offsetClass.equals("main")) {
                MainScdOffset39 mainScdOffset39 = new MainScdOffset39(offset, i, i + 1);
                offsetList.add(mainScdOffset39);
            } else if (offsetClass.equals("sub")) {
                SubScdOffset41 subScdOffset41 = new SubScdOffset41(offset, i, i + 1);
                offsetList.add(subScdOffset41);
            } else if (offsetClass.equals("extra")) {
                ExtraScdOffset43 extraScdOffset43 = new ExtraScdOffset43(offset, i, i + 1);
                offsetList.add(extraScdOffset43);
            } else if (offsetClass.equals("msg")) {
                MainMsgOffset45 mainMsgOffset45 = new MainMsgOffset45(offset, i, i + 1);
                offsetList.add(mainMsgOffset45);
            }
        }

        return offsetList;
    }

    public List<Object> fetchData(int startOffset, int beginOfNextOffsetData, List<String> rdtList, List<Object> dataOffsets) {
        int offsetStartData = startOffset;
        int currentOffsetData;

        int dataStart;
        int dataEnd;

        List<Object> listOfDataObjects = new ArrayList<>();

        for (int i = 0; i < dataOffsets.size(); i++) {
            if (dataOffsets.size() > i + 1) {
                //It is the beginning of the next data element
                dataEnd = findEndOfData(dataOffsets.get(i + 1), offsetStartData);
            } else {
                //Its the offset before the next object element starts
                dataEnd = beginOfNextOffsetData - 1;
            }
            if (dataOffsets.get(i) instanceof MainScdOffset39) {
                MainScdOffset39 mainScdOffset39 = (MainScdOffset39) dataOffsets.get(i);
                currentOffsetData = mainScdOffset39.getCorrespondingScdOffset();
                dataStart = offsetStartData + currentOffsetData;
                String mainScdData = fetchRdtDataFromOffsets(dataStart, dataEnd + 1, rdtList);
                MainScd40 mainScd40 = new MainScd40(mainScdData, dataStart, dataEnd);
                listOfDataObjects.add(mainScd40);
            } else if (dataOffsets.get(i) instanceof SubScdOffset41) {
                SubScdOffset41 subScdOffset41 = (SubScdOffset41) dataOffsets.get(i);
                currentOffsetData = subScdOffset41.getCorrespondingScdOffset();
                dataStart = offsetStartData + currentOffsetData;
                String subScdData = fetchRdtDataFromOffsets(dataStart, dataEnd +1, rdtList);
                SubScd42 subScd42 = new SubScd42(subScdData, dataStart, dataEnd);
                listOfDataObjects.add(subScd42);
            } else if (dataOffsets.get(i) instanceof ExtraScdOffset43) {
                ExtraScdOffset43 extraScdOffset43 = (ExtraScdOffset43) dataOffsets.get(i);
                currentOffsetData = extraScdOffset43.getCorrespondingScdOffset();
                dataStart = offsetStartData + currentOffsetData;
                String extraScdData = fetchRdtDataFromOffsets(dataStart, dataEnd +1, rdtList);
                ExtraScd44 extraScd44 = new ExtraScd44(extraScdData, dataStart, dataEnd);
                listOfDataObjects.add(extraScd44);
            } else if (dataOffsets.get(i) instanceof MainMsgOffset45) {
                MainMsgOffset45 mainMsgOffset45 = (MainMsgOffset45) dataOffsets.get(i);
                currentOffsetData = mainMsgOffset45.getMainMsgOffset();
                dataStart = offsetStartData + currentOffsetData;
                String mainMsgData = fetchRdtDataFromOffsets(dataStart, dataEnd +1, rdtList);
                MainMsg46 mainMsg46 = new MainMsg46(mainMsgData, dataStart, dataEnd);
                listOfDataObjects.add(mainMsg46);
            }
        }

        return listOfDataObjects;
    }

    private int findEndOfData(Object scdDataOffset, int offsetStartData) {
        int dataEnd = 0;
        if (scdDataOffset instanceof MainScdOffset39) {
            MainScdOffset39 mainScdOffset39 = (MainScdOffset39) scdDataOffset;
            dataEnd = mainScdOffset39.getCorrespondingScdOffset();
        } else if (scdDataOffset instanceof SubScdOffset41) {
            SubScdOffset41 subScdOffset41 = (SubScdOffset41) scdDataOffset;
            dataEnd = subScdOffset41.getCorrespondingScdOffset();
        } else if (scdDataOffset instanceof ExtraScdOffset43) {
            ExtraScdOffset43 extraScdOffset43 = (ExtraScdOffset43) scdDataOffset;
            dataEnd = extraScdOffset43.getCorrespondingScdOffset();
        } else if (scdDataOffset instanceof MainMsgOffset45) {
            MainMsgOffset45 mainMsgOffset45 = (MainMsgOffset45) scdDataOffset;
            dataEnd = mainMsgOffset45.getMainMsgOffset();
        }

        //It is the beginning of the next scd data
        dataEnd = offsetStartData + dataEnd - 1;

        return dataEnd;
    }

    public void parseScdElements(List<Object> scdDataElements) {
        for (int i = 0; i < scdDataElements.size(); i++) {
            ScdUtilities.getInstance().setSingleScdDataElements(scdDataElements.get(i));
        }
    }

    public List<Object> fetchMainScdDataElements(List<Object> rdtObjectList) {
        List<Object> mainScdObjectList = new ArrayList<>();
        for (int i = 0; i < rdtObjectList.size(); i++) {
            if (rdtObjectList.get(i) instanceof MainScd40) {
                mainScdObjectList.add(rdtObjectList.get(i));
            }
        }

        return mainScdObjectList;
    }

    public List<Object> fetchSubScdDataElements(List<Object> rdtObjectList) {
        List<Object> subScdObjectList = new ArrayList<>();
        for (int i = 0; i < rdtObjectList.size(); i++) {
            if (rdtObjectList.get(i) instanceof SubScd42) {
                subScdObjectList.add(rdtObjectList.get(i));
            }
        }

        return subScdObjectList;
    }

    public List<Object> fetchExtraScdDataElements(List<Object> rdtObjectList) {
        List<Object> extraScdObjectList = new ArrayList<>();
        for (int i = 0; i < rdtObjectList.size(); i++) {
            if (rdtObjectList.get(i) instanceof ExtraScd44) {
                extraScdObjectList.add(rdtObjectList.get(i));
            }
        }

        return extraScdObjectList;
    }
}
