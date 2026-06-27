package baubabi.reeditor.core.rdt.scd._72_op48_super_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SuperSet_superSet_48 {
    //16 Byte

    /*
        Something, no clue...
    */

    //1 byte
    private final String OPCODE = "48";

    //15 byte - unknown data
    private final String[] superSetUnknown = new String[15];

    private final int size = 16;

    private final String displayName = OPCODE + " - super set";

    public SuperSet_superSet_48(String superSetValue) {
        if(superSetValue != null) {
            this.superSetUnknown[0] = superSetValue.substring(2, 4);
            this.superSetUnknown[1] = superSetValue.substring(4, 6);
            this.superSetUnknown[2] = superSetValue.substring(6, 8);
            this.superSetUnknown[3] = superSetValue.substring(8, 10);
            this.superSetUnknown[4] = superSetValue.substring(10, 12);
            this.superSetUnknown[5] = superSetValue.substring(12, 14);
            this.superSetUnknown[6] = superSetValue.substring(14, 16);
            this.superSetUnknown[7] = superSetValue.substring(16, 18);
            this.superSetUnknown[8] = superSetValue.substring(18, 20);
            this.superSetUnknown[9] = superSetValue.substring(20, 22);
            this.superSetUnknown[10] = superSetValue.substring(22, 24);
            this.superSetUnknown[11] = superSetValue.substring(24, 26);
            this.superSetUnknown[12] = superSetValue.substring(26, 28);
            this.superSetUnknown[13] = superSetValue.substring(28, 30);
            this.superSetUnknown[14] = superSetValue.substring(30, 32);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + superSetUnknown[0];
        newScdDataByteString = newScdDataByteString + superSetUnknown[1];
        newScdDataByteString = newScdDataByteString + superSetUnknown[2];
        newScdDataByteString = newScdDataByteString + superSetUnknown[3];
        newScdDataByteString = newScdDataByteString + superSetUnknown[4];
        newScdDataByteString = newScdDataByteString + superSetUnknown[5];
        newScdDataByteString = newScdDataByteString + superSetUnknown[6];
        newScdDataByteString = newScdDataByteString + superSetUnknown[7];
        newScdDataByteString = newScdDataByteString + superSetUnknown[8];
        newScdDataByteString = newScdDataByteString + superSetUnknown[9];
        newScdDataByteString = newScdDataByteString + superSetUnknown[10];
        newScdDataByteString = newScdDataByteString + superSetUnknown[11];
        newScdDataByteString = newScdDataByteString + superSetUnknown[12];
        newScdDataByteString = newScdDataByteString + superSetUnknown[13];
        newScdDataByteString = newScdDataByteString + superSetUnknown[14];

        return newScdDataByteString;
    }
}
