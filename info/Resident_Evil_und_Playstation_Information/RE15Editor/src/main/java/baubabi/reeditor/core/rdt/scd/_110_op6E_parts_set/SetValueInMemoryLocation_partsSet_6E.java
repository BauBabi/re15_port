package baubabi.reeditor.core.rdt.scd._110_op6E_parts_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetValueInMemoryLocation_partsSet_6E {
    //6 Byte

    /*
        Set value in some memory location.
    */

    //1 byte
    private final String OPCODE = "6E"; /* 0x6e */

    //1 byte - a dummy value for it
    private final String[] setValueInMemoryLocationDummy = new String[1];

    //2 byte - unknown value - an array of 2 with 1 byte each
    private final String[] setValueInMemoryLocationUnknown = new String[2];

    //2 byte - the value to set in Memory
    private final String[] setValueInMemoryLocationValue = new String[2];

    private final int size = 6;

    private final String displayName = OPCODE + " - set value in memory location";

    public SetValueInMemoryLocation_partsSet_6E(String setValueInMemoryLocationValue) {
        if(setValueInMemoryLocationValue != null) {
            this.setValueInMemoryLocationDummy[0] = setValueInMemoryLocationValue.substring(2, 4);
            this.setValueInMemoryLocationUnknown[0] = setValueInMemoryLocationValue.substring(4, 6);
            this.setValueInMemoryLocationUnknown[1] = setValueInMemoryLocationValue.substring(6, 8);
            this.setValueInMemoryLocationValue[0] = setValueInMemoryLocationValue.substring(8, 10);
            this.setValueInMemoryLocationValue[1] = setValueInMemoryLocationValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setValueInMemoryLocationDummy[0];
        newScdDataByteString = newScdDataByteString + setValueInMemoryLocationUnknown[0];
        newScdDataByteString = newScdDataByteString + setValueInMemoryLocationUnknown[1];
        newScdDataByteString = newScdDataByteString + setValueInMemoryLocationValue[0];
        newScdDataByteString = newScdDataByteString + setValueInMemoryLocationValue[1];

        return newScdDataByteString;
    }
}
