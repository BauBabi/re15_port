package baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.common;

import baubabi.reeditor.core.general.utils.ReflectionUtilities;
import baubabi.reeditor.core.rdt.RdtProcessing;
import baubabi.reeditor.core.rdt._08_snd0_edt_address.Snd0EdtAddress08;
import baubabi.reeditor.core.rdt._09_snd0_vh_address.Snd0VhAddress09;
import baubabi.reeditor.core.rdt._10_snd0_vb_address.Snd0VbAddress10;
import baubabi.reeditor.core.rdt._11_snd1_edt_address.Snd1EdtAddress11;
import baubabi.reeditor.core.rdt._12_snd1_vh_address.Snd1VhAddress12;
import baubabi.reeditor.core.rdt._13_snd1_vb_address.Snd1VbAddress13;
import baubabi.reeditor.core.rdt._21_message_msg_address.MessageMsgAddress21;
import baubabi.reeditor.core.rdt._22_main_scd_offset_address.MainScdOffsetAddress22;
import baubabi.reeditor.core.rdt._23_sub_scd_offset_address.SubScdOffsetAddress23;
import baubabi.reeditor.core.rdt._24_extra_scd_offset_address.ExtraScdOffsetAddress24;
import baubabi.reeditor.core.rdt._25_effect_esp_address.EffectEspAddress25;
import baubabi.reeditor.core.rdt._26_something_evtl_eff_address.SomethingEvtlEffAddress26;
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
import baubabi.reeditor.ui.main.panels._02_thirdlayer._02_scd_main_panel.handler.utilities.HandlerUtilities;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class AddRemoveHandlerCommons {

    List<Object> addressElements;

    List<Object> offsetElements;

    List<Object> scdAndMsgElements;

    List<Object> remainingElements;

    public AddRemoveHandlerCommons() {
        addressElements = new ArrayList<>();
        offsetElements = new ArrayList<>();
        scdAndMsgElements = new ArrayList<>();
        remainingElements = new ArrayList<>();

        for (int i = 0; i < RdtProcessing.getInstance().getRdtObjectList().size(); i++) {
            if ((RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd0EdtAddress08)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd0VhAddress09)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd0VbAddress10)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd1EdtAddress11)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd1VhAddress12)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd1VbAddress13)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof MessageMsgAddress21)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof MainScdOffsetAddress22)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof SubScdOffsetAddress23)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof ExtraScdOffsetAddress24)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof EffectEspAddress25)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof SomethingEvtlEffAddress26)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof EspTimAddress27)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof ModelTimAddress28)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof AnimationRbjAddress29)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof ModelTimElementAddress31)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof ModelMd1ElementAddress32)) {
                addressElements.add(RdtProcessing.getInstance().getRdtObjectList().get(i));
            }
            else if((RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof MainScdOffset39)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof SubScdOffset41)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof ExtraScdOffset43)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof MainMsgOffset45)){
                offsetElements.add(RdtProcessing.getInstance().getRdtObjectList().get(i));
            }
            else if((RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof MainScd40)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof SubScd42)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof ExtraScd44)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof MainMsg46)){
                scdAndMsgElements.add(RdtProcessing.getInstance().getRdtObjectList().get(i));
            }
            else if((RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof AnimationRbj47)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof ModelMd148)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof EffectEsp58)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd0Edt49)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof EffectEsp58)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd0Vh50)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd1Edt51)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd1Vh52)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd0Vb53)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd1Vb54)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd0Snd55)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof Snd1Snd56)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof EspTim59)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof UnknownData60)
                    || (RdtProcessing.getInstance().getRdtObjectList().get(i) instanceof ModelTim57)){
                remainingElements.add(RdtProcessing.getInstance().getRdtObjectList().get(i));
            }
        }
    }

    public void modifyElement(Object elementToModify, Object scdElementTab, boolean isRemove) {

        int sizeOfElementToModify = callIntMethodForElement(elementToModify, "getSize");

        Object offsetElementBelongingToScd = fetchCorrespondingScdOffsetElementForScdElementTab(scdElementTab);

        boolean isLastScdElementOfContainer = false;
        int sizeOfOffsetElementBelongingToScd = 0;
        int numOfSingleScdDataElementsOfScdElementTab = 0;

        if(isRemove) {
            sizeOfOffsetElementBelongingToScd = callIntMethodForElement(offsetElementBelongingToScd, "getElementSize");

            numOfSingleScdDataElementsOfScdElementTab = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnObjectList(scdElementTab, "getSingleScdDataElements", this.getClass().getCanonicalName()).size();

            isLastScdElementOfContainer = numOfSingleScdDataElementsOfScdElementTab == 1;

            updateAddressElementOfScdElementTab(scdElementTab);
        }

        updateAddressElements(isLastScdElementOfContainer, sizeOfOffsetElementBelongingToScd, sizeOfElementToModify, scdElementTab, isRemove);

        updateSuccessorOffsetsOfScdElementTabElement(offsetElementBelongingToScd, isLastScdElementOfContainer, sizeOfElementToModify, isRemove);

        updateSuccessorOfScdElementTabElement(scdElementTab, offsetElementBelongingToScd, isLastScdElementOfContainer, sizeOfElementToModify, isRemove);

        updateRemainingElements(offsetElementBelongingToScd, isLastScdElementOfContainer, sizeOfElementToModify, isRemove);

        updateScdTabElement(scdElementTab, elementToModify, isLastScdElementOfContainer, offsetElementBelongingToScd, sizeOfElementToModify, isRemove);
    }

    public int callIntMethodForElement(Object element, String method) {
        int result = 0;
        result = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnInt(element, method, this.getClass().getCanonicalName());

        return result;
    }

    public Object fetchCorrespondingScdOffsetElementForScdElementTab(Object scdElementTab) {
        int scdElementTabStartAddress = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnInt(scdElementTab, "getRdtOffsetPositionStart", this.getClass().getCanonicalName());
        Object scdOffsetElementBelongingToTabElement = null;
        boolean isFirstOffset = false;
        int startOffsetForCalculation = 0;

        for (int i = 0; i < RdtProcessing.getInstance().getRdtObjectList().size(); i++) {
            if (isObjectAtIndexCorrespondingOffsetElement(scdElementTab, RdtProcessing.getInstance().getRdtObjectList().get(i))) {
                if(!isFirstOffset) {
                    startOffsetForCalculation = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnInt(RdtProcessing.getInstance().getRdtObjectList().get(i), "getRdtOffsetPositionStart", this.getClass().getCanonicalName());
                    isFirstOffset = true;
                }

                int startPositionOfBelongingScd = startOffsetForCalculation
                        + ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnInt(RdtProcessing.getInstance().getRdtObjectList().get(i), "getCorrespondingScdOffset", this.getClass().getCanonicalName());

                //This mainScdOffset belong to the mainScd we delete the element in.
                if (scdElementTabStartAddress == startPositionOfBelongingScd) {
                    scdOffsetElementBelongingToTabElement = RdtProcessing.getInstance().getRdtObjectList().get(i);
                    break;
                }
            }
        }

        return scdOffsetElementBelongingToTabElement;
    }

    public boolean isObjectAtIndexCorrespondingOffsetElement(Object scdElementTab, Object scdElementToCheck) {
        Object objectToCheck = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnObject(scdElementTab, "getParentScdOffsetElement", this.getClass().getCanonicalName());

        return scdElementToCheck.getClass().equals(objectToCheck);
    }

    public void updateAddressElements(boolean isLastScdElementOfContainer, int offsetElementSize, int sizeOfElementToModify, Object scdElementTabToCheck, boolean isRemove) {
        Object objectToCheck = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnObject(scdElementTabToCheck, "getScdAddressElement", this.getClass().getCanonicalName());
        HashMap<Object, Integer> orderMap = new HashMap<Object, Integer>();
        orderMap.put(MainScdOffsetAddress22.class, 1);
        orderMap.put(SubScdOffsetAddress23.class, 2);
        orderMap.put(ExtraScdOffsetAddress24.class, 3);
        int orderNumberToCheck = orderMap.get(objectToCheck);

        for (int i = 0; i < addressElements.size(); i++) {
            //Check if address of scd element is not the offset element deleted from itself (if sub is deleted, not sub offset, etc)
            if (!addressElements.get(i).getClass().equals(objectToCheck)) {
                //Checks if address scd element is smaller than the one that is deleted. For example, when a sub scd element is deleted, the address of main the main elements is not affected.
                if(orderMap.get(addressElements.get(i).getClass()) != null) {
                    if(orderMap.get(addressElements.get(i).getClass()) < orderNumberToCheck) {
                        continue;
                    }
                }

                int addressOfElement = callIntMethodForElement(addressElements.get(i), "getAddress");
                if (addressOfElement != 0) {
                    if(isRemove) {
                        if (isLastScdElementOfContainer) {
                            setNewAddressForElement(addressElements.get(i), offsetElementSize, isRemove);
                        }
                    }
                    setNewAddressForElement(addressElements.get(i), sizeOfElementToModify, isRemove);
                }
            }
        }
    }

    public void setNewAddressForElement(Object element, int addressModificator, boolean isRemove) {
        int originalAddress = callIntMethodForElement(element, "getAddress");
        int newAddress = modifyAddressCalculation(originalAddress, addressModificator, isRemove);


        ReflectionUtilities.getInstance().executeMethodForClassObjectWithIntegerParameter(element, "setAddress", this.getClass().getCanonicalName(), newAddress);
    }

    public void setNewOffsetPositionStartAndEndForElement(Object element, int addressModificator, boolean isRemove) {
        int originalPositionStart = callIntMethodForElement(element, "getRdtOffsetPositionStart");
        int originalPositionEnd = callIntMethodForElement(element, "getRdtOffsetPositionEnd");
        int newPositionStart = modifyAddressCalculation(originalPositionStart, addressModificator, isRemove);
        int newPositionEnd = modifyAddressCalculation(originalPositionEnd, addressModificator, isRemove);
        ReflectionUtilities.getInstance().executeMethodForClassObjectWithIntegerParameter(element, "setRdtOffsetPositionStart", this.getClass().getCanonicalName(), newPositionStart);
        ReflectionUtilities.getInstance().executeMethodForClassObjectWithIntegerParameter(element, "setRdtOffsetPositionEnd", this.getClass().getCanonicalName(), newPositionEnd);
    }

    //Only for delete
    public void updateAddressElementOfScdElementTab(Object scdElementTab) {
        Object addressElementClassOfScdElementTab = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnObject(scdElementTab, "getScdAddressElement", this.getClass().getCanonicalName());
        Object addressElementOfScdElementTab = null;

        //Find Address Element for Current Scd Tab
        for(int i = 0; i < addressElements.size(); i++) {
            if(addressElements.get(i).getClass().equals(addressElementClassOfScdElementTab)) {
                addressElementOfScdElementTab = addressElements.get(i);
                break;
            }
        }

        //Address of Scd Offsets
        int addressOfOffsetElements = callIntMethodForElement(addressElementOfScdElementTab, "getAddress");

        if (addressOfOffsetElements != 0) {

            //Go through Rdt Elements, check if there is another instance of Scd of this type
            boolean anotherScdExist = false;
            int scdPositionElementInRdtObjectList = RdtProcessing.getInstance().getRdtObjectList().indexOf(scdElementTab);
            for (int i = 0; i < RdtProcessing.getInstance().getRdtObjectList().size(); i++) {
                if (RdtProcessing.getInstance().getRdtObjectList().get(i).getClass().equals(scdElementTab.getClass())) {
                    if (i != scdPositionElementInRdtObjectList) {
                        anotherScdExist = true;
                        break;
                    }
                }
            }

            //If not, set the scd address Pointer to 0
            if (!anotherScdExist) {
                ReflectionUtilities.getInstance().executeMethodForClassObjectWithIntegerParameter(addressElementOfScdElementTab, "setAddress", this.getClass().getCanonicalName(), 0);
            }
        }
    }

    public void updateSuccessorOffsetsOfScdElementTabElement(Object offsetElementBelongingToScd, boolean isLastScdElementOfContainer, int scdElementSize, boolean isRemove) {
        int scdOffsetPositionInRdtObjectList = RdtProcessing.getInstance().getRdtObjectList().indexOf(offsetElementBelongingToScd);
        for(int i = 0; i < offsetElements.size(); i++) {
            int positionInRdtObjectList = RdtProcessing.getInstance().getRdtObjectList().indexOf(offsetElements.get(i));
            if(positionInRdtObjectList > scdOffsetPositionInRdtObjectList) {
                if(callIntMethodForElement(offsetElements.get(i), "getRdtOffsetPositionStart") != 0) {
                    if(isRemove) {
                        if(isLastScdElementOfContainer) {
                            //Subtract the Offset Element Size
                            int originalOffset = callIntMethodForElement(offsetElements.get(i), "getCorrespondingScdOffset");
                            int offsetElementSize = callIntMethodForElement(offsetElementBelongingToScd, "getElementSize");
                            int newOffset = modifyAddressCalculation(originalOffset, offsetElementSize, isRemove);
                            setNewOffsetPositionStartAndEndForElement(offsetElements.get(i), offsetElementSize, isRemove);
                            ReflectionUtilities.getInstance().executeMethodForClassObjectWithIntegerParameter(offsetElements.get(i), "setCorrespondingScdOffset", this.getClass().getCanonicalName(), newOffset);
                        }
                    }

                    //Subtract or Add the Element to modify
                    if(offsetElements.get(i).getClass().equals(offsetElementBelongingToScd.getClass())) {
                        int originalOffset = callIntMethodForElement(offsetElements.get(i), "getCorrespondingScdOffset");
                        int newOffset = modifyAddressCalculation(originalOffset, scdElementSize, isRemove);
                        ReflectionUtilities.getInstance().executeMethodForClassObjectWithIntegerParameter(offsetElements.get(i), "setCorrespondingScdOffset", this.getClass().getCanonicalName(), newOffset);
                    }
                    else {
                        setNewOffsetPositionStartAndEndForElement(offsetElements.get(i), scdElementSize, isRemove);
                    }
                }
            }
        }
    }

    public void updateSuccessorOfScdElementTabElement(Object scdElementTab, Object offsetElementBelongingToScd, boolean isLastScdElementOfContainer, int scdElementSize, boolean isRemove) {
        int scdPositionInRdtObjectList = RdtProcessing.getInstance().getRdtObjectList().indexOf(scdElementTab);
        for(int i = 0; i < scdAndMsgElements.size(); i++) {
            int positionInRdtObjectList = RdtProcessing.getInstance().getRdtObjectList().indexOf(offsetElements.get(i));
            if (positionInRdtObjectList > scdPositionInRdtObjectList) {
                UpdateElements(scdAndMsgElements.get(i), isLastScdElementOfContainer, offsetElementBelongingToScd, scdElementSize, isRemove);
            }
        }
    }

    public void updateRemainingElements(Object offsetElementBelongingToScd, boolean isLastScdElementOfContainer, int sizeOfElementToModify, boolean isRemove) {
        for(int i = 0; i < remainingElements.size(); i++) {
            if(callIntMethodForElement(remainingElements.get(i), "getRdtOffsetPositionStart") != 0) {
                UpdateElements(remainingElements.get(i), isLastScdElementOfContainer, offsetElementBelongingToScd, sizeOfElementToModify, isRemove);
            }
        }
    }

    private void UpdateElements(Object elementToUpdate, boolean isLastScdElementOfContainer, Object offsetElementBelongingToScd, int sizeOfElementToModify, boolean isRemove) {
        if(isRemove) {
            if (isLastScdElementOfContainer) {
                int offsetSize = callIntMethodForElement(offsetElementBelongingToScd, "getElementSize");
                setNewOffsetPositionStartAndEndForElement(elementToUpdate, offsetSize, isRemove);
            }
        }
        setNewOffsetPositionStartAndEndForElement(elementToUpdate, sizeOfElementToModify, isRemove);
    }

    private void updateScdTabElement(Object scdElementTab, Object objectToModify, boolean isLastScdElementOfContainer, Object offsetElementBelongingToScd, int sizeOfElementToModify, boolean isRemove) {
        List<Object> singleScdDataElements = ReflectionUtilities.getInstance().executeMethodForClassObjectAndReturnObjectList(scdElementTab, "getSingleScdDataElements", this.getClass().getCanonicalName());
        if(isRemove) {
            singleScdDataElements.remove(objectToModify);
        }
        else {
            singleScdDataElements.add(objectToModify);
        }

        if(isRemove) {
            if(isLastScdElementOfContainer) {
                RdtProcessing.getInstance().getRdtObjectList().remove(offsetElementBelongingToScd);
                RdtProcessing.getInstance().getRdtObjectList().remove(scdElementTab);
            }
            else {
                setRdtOffsetPositionEnd(scdElementTab, sizeOfElementToModify, true);
            }
        }
        else {
            setRdtOffsetPositionEnd(scdElementTab, sizeOfElementToModify, false);
        }

        String newMainScdData = HandlerUtilities.getInstance().fetchAndReplaceScdElementData(singleScdDataElements);
        ReflectionUtilities.getInstance().executeMethodForClassObjectWithStringParameter(scdElementTab, "setScdData", this.getClass().getCanonicalName(), newMainScdData);
    }

    private void setRdtOffsetPositionEnd(Object scdElementTab, int sizeOfElementToModify, boolean isRemove) {
        int oldPositionEnd = callIntMethodForElement(scdElementTab, "getRdtOffsetPositionEnd");
        int newPositionEnd = modifyAddressCalculation(oldPositionEnd, sizeOfElementToModify, isRemove);

        ReflectionUtilities.getInstance().executeMethodForClassObjectWithIntegerParameter(scdElementTab, "setRdtOffsetPositionEnd", this.getClass().getCanonicalName(), newPositionEnd);
    }

    private int modifyAddressCalculation(int calculationValue1, int calculationValue2, boolean isRemove) {
        if(isRemove) {
            return (calculationValue1 - calculationValue2);
        }
        else {
            return (calculationValue1 + calculationValue2);
        }
    }
}