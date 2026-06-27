package baubabi.reeditor.core.rdt.scd._54_op36_se_on;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SeOn_seOn_36 {
    //12 Byte

    /*
        Fully unknown what it does
    */

    //1 byte
    private final String OPCODE = "36"; /* 0x36 */

    //1 byte - unknown value 1
    private final String[] seOnUnknown1 = new String[1];

    //4 byte - unknown value 2 - An Array of two with 2 byte each
    private final String[] seOnUnknown2 = new String[4];

    //6 byte - unknown value 3 - An Array of three with 2 byte each
    private final String[] seOnUnknown3 = new String[6];

    private final int size = 12;

    private final String displayName = "36 - se on";


    public SeOn_seOn_36(String seOnValue) {
        if(seOnValue != null) {
            this.seOnUnknown1[0] = seOnValue.substring(2,4);
            this.seOnUnknown2[0] = seOnValue.substring(4,6);
            this.seOnUnknown2[1] = seOnValue.substring(6,8);
            this.seOnUnknown2[2] = seOnValue.substring(8,10);
            this.seOnUnknown2[3] = seOnValue.substring(10,12);
            this.seOnUnknown3[0] = seOnValue.substring(12,14);
            this.seOnUnknown3[1] = seOnValue.substring(14,16);
            this.seOnUnknown3[2] = seOnValue.substring(16,18);
            this.seOnUnknown3[3] = seOnValue.substring(18,20);
            this.seOnUnknown3[4] = seOnValue.substring(20,22);
            this.seOnUnknown3[5] = seOnValue.substring(22,24);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + seOnUnknown1[0];
        newScdDataByteString = newScdDataByteString + seOnUnknown2[0];
        newScdDataByteString = newScdDataByteString + seOnUnknown2[1];
        newScdDataByteString = newScdDataByteString + seOnUnknown2[2];
        newScdDataByteString = newScdDataByteString + seOnUnknown2[3];
        newScdDataByteString = newScdDataByteString + seOnUnknown3[0];
        newScdDataByteString = newScdDataByteString + seOnUnknown3[1];
        newScdDataByteString = newScdDataByteString + seOnUnknown3[2];
        newScdDataByteString = newScdDataByteString + seOnUnknown3[3];
        newScdDataByteString = newScdDataByteString + seOnUnknown3[4];
        newScdDataByteString = newScdDataByteString + seOnUnknown3[5];

        return newScdDataByteString;
    }
}
