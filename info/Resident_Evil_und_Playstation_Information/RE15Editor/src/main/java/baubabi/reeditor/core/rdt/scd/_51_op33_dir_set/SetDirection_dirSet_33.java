package baubabi.reeditor.core.rdt.scd._51_op33_dir_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetDirection_dirSet_33 {
    //8 Byte

    /*
        Set direction
    */

    //1 byte
    private final String OPCODE = "33";

    //7 byte - The direction data
    private final String[] setDirectionData = new String[7];

    private final int size = 8;

    private final String displayName = "33 - set direction";


    public SetDirection_dirSet_33(String setDirectionValue) {
        if(setDirectionValue != null) {
            this.setDirectionData[0] = setDirectionValue.substring(2, 4);
            this.setDirectionData[1] = setDirectionValue.substring(4, 6);
            this.setDirectionData[2] = setDirectionValue.substring(6, 8);
            this.setDirectionData[3] = setDirectionValue.substring(8, 10);
            this.setDirectionData[4] = setDirectionValue.substring(10, 12);
            this.setDirectionData[5] = setDirectionValue.substring(12, 14);
            this.setDirectionData[6] = setDirectionValue.substring(14, 16);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setDirectionData[0];
        newScdDataByteString = newScdDataByteString + setDirectionData[1];
        newScdDataByteString = newScdDataByteString + setDirectionData[2];
        newScdDataByteString = newScdDataByteString + setDirectionData[3];
        newScdDataByteString = newScdDataByteString + setDirectionData[4];
        newScdDataByteString = newScdDataByteString + setDirectionData[5];
        newScdDataByteString = newScdDataByteString + setDirectionData[6];

        return newScdDataByteString;
    }
}
