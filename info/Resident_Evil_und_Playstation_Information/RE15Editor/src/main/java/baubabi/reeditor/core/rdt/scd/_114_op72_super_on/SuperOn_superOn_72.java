package baubabi.reeditor.core.rdt.scd._114_op72_super_on;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SuperOn_superOn_72 {
    //16 Byte

    /*
        Don't know the function of it
    */

    //1 byte
    private final String OPCODE = "72";

    //15 byte - the super_on data
    private final String[] superOnData = new String[15];

    private final int size = 16;

    private final String displayName = OPCODE + " - super on";

    public SuperOn_superOn_72(String superOnValue) {
        if(superOnValue != null) {
            this.superOnData[0] = superOnValue.substring(2, 4);
            this.superOnData[1] = superOnValue.substring(4, 6);
            this.superOnData[2] = superOnValue.substring(6, 8);
            this.superOnData[3] = superOnValue.substring(8, 10);
            this.superOnData[4] = superOnValue.substring(10, 12);
            this.superOnData[5] = superOnValue.substring(12, 14);
            this.superOnData[6] = superOnValue.substring(14, 16);
            this.superOnData[7] = superOnValue.substring(16, 18);
            this.superOnData[8] = superOnValue.substring(18, 20);
            this.superOnData[9] = superOnValue.substring(20, 22);
            this.superOnData[10] = superOnValue.substring(22, 24);
            this.superOnData[11] = superOnValue.substring(24, 26);
            this.superOnData[12] = superOnValue.substring(26, 28);
            this.superOnData[13] = superOnValue.substring(28, 30);
            this.superOnData[14] = superOnValue.substring(30, 32);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + superOnData[0];
        newScdDataByteString = newScdDataByteString + superOnData[1];
        newScdDataByteString = newScdDataByteString + superOnData[2];
        newScdDataByteString = newScdDataByteString + superOnData[3];
        newScdDataByteString = newScdDataByteString + superOnData[4];
        newScdDataByteString = newScdDataByteString + superOnData[5];
        newScdDataByteString = newScdDataByteString + superOnData[6];
        newScdDataByteString = newScdDataByteString + superOnData[7];
        newScdDataByteString = newScdDataByteString + superOnData[8];
        newScdDataByteString = newScdDataByteString + superOnData[9];
        newScdDataByteString = newScdDataByteString + superOnData[10];
        newScdDataByteString = newScdDataByteString + superOnData[11];
        newScdDataByteString = newScdDataByteString + superOnData[12];
        newScdDataByteString = newScdDataByteString + superOnData[13];
        newScdDataByteString = newScdDataByteString + superOnData[14];

        return newScdDataByteString;
    }
}
