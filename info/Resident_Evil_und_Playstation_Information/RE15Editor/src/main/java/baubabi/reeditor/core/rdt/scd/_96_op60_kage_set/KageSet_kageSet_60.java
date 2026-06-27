package baubabi.reeditor.core.rdt.scd._96_op60_kage_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class KageSet_kageSet_60 {
    //14 Byte

    /*
        Something, don't know
    */

    //1 byte
    private final String OPCODE = "60"; /* 0x60 */

    //5 byte - unknown value 1 - array of 5 with 1 byte each
    private final String[] kageSetUnknown1 = new String[5];

    //8 byte - unknown value 2 - array of 4 with 2 byte each
    private final String[] kageSetUnknown2 = new String[8];

    private final int size = 14;

    private final String displayName = OPCODE + " - kage set";

    public KageSet_kageSet_60(String kageSetValue) {
        if(kageSetValue != null) {
            this.kageSetUnknown1[0] = kageSetValue.substring(2, 4);
            this.kageSetUnknown1[1] = kageSetValue.substring(4, 6);
            this.kageSetUnknown1[2] = kageSetValue.substring(6, 8);
            this.kageSetUnknown1[3] = kageSetValue.substring(8, 10);
            this.kageSetUnknown1[4] = kageSetValue.substring(10, 12);
            this.kageSetUnknown2[0] = kageSetValue.substring(12, 14);
            this.kageSetUnknown2[1] = kageSetValue.substring(14, 16);
            this.kageSetUnknown2[2] = kageSetValue.substring(16, 18);
            this.kageSetUnknown2[3] = kageSetValue.substring(18, 20);
            this.kageSetUnknown2[4] = kageSetValue.substring(20, 22);
            this.kageSetUnknown2[5] = kageSetValue.substring(22, 24);
            this.kageSetUnknown2[6] = kageSetValue.substring(24, 26);
            this.kageSetUnknown2[7] = kageSetValue.substring(26, 28);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + kageSetUnknown1[0];
        newScdDataByteString = newScdDataByteString + kageSetUnknown1[1];
        newScdDataByteString = newScdDataByteString + kageSetUnknown1[2];
        newScdDataByteString = newScdDataByteString + kageSetUnknown1[3];
        newScdDataByteString = newScdDataByteString + kageSetUnknown1[4];
        newScdDataByteString = newScdDataByteString + kageSetUnknown2[0];
        newScdDataByteString = newScdDataByteString + kageSetUnknown2[1];
        newScdDataByteString = newScdDataByteString + kageSetUnknown2[2];
        newScdDataByteString = newScdDataByteString + kageSetUnknown2[3];
        newScdDataByteString = newScdDataByteString + kageSetUnknown2[4];
        newScdDataByteString = newScdDataByteString + kageSetUnknown2[5];
        newScdDataByteString = newScdDataByteString + kageSetUnknown2[6];
        newScdDataByteString = newScdDataByteString + kageSetUnknown2[7];

        return newScdDataByteString;
    }
}
