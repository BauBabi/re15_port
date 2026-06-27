package baubabi.reeditor.core.rdt.scd._50_op32_pos_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetPosition_posSet_32 {
    //8 Byte

    /*
        Set 3 words
    */

    //1 byte
    private final String OPCODE = "32";

    //7 byte - The position data
    private final String[] setPositionData = new String[7];

    private final int size = 8;

    private final String displayName = "32 - set three words";

    public SetPosition_posSet_32(String setPositionValue) {
        if(setPositionValue != null) {
            this.setPositionData[0] = setPositionValue.substring(2, 4);
            this.setPositionData[1] = setPositionValue.substring(4, 6);
            this.setPositionData[2] = setPositionValue.substring(6, 8);
            this.setPositionData[3] = setPositionValue.substring(8, 10);
            this.setPositionData[4] = setPositionValue.substring(10, 12);
            this.setPositionData[5] = setPositionValue.substring(12, 14);
            this.setPositionData[6] = setPositionValue.substring(14, 16);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setPositionData[0];
        newScdDataByteString = newScdDataByteString + setPositionData[1];
        newScdDataByteString = newScdDataByteString + setPositionData[2];
        newScdDataByteString = newScdDataByteString + setPositionData[3];
        newScdDataByteString = newScdDataByteString + setPositionData[4];
        newScdDataByteString = newScdDataByteString + setPositionData[5];
        newScdDataByteString = newScdDataByteString + setPositionData[6];

        return newScdDataByteString;
    }
}
