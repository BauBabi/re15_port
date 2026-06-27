package baubabi.reeditor.core.rdt.scd._100_op64_sce_espr_on2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceEsprOn2_sceEsprOn2_64 {
    //16 Byte

    /*
        Something, don't know
    */

    //1 byte
    private final String OPCODE = "64";

    //15 byte - unknown value
    private final String[] sceEsprOn2Unknown = new String[15];

    private final int size = 16;

    private final String displayName = OPCODE + " - sce espr on 2";

    public SceEsprOn2_sceEsprOn2_64(String sceEsprOn2Value) {
        if(sceEsprOn2Value != null) {
            this.sceEsprOn2Unknown[0] = sceEsprOn2Value.substring(2, 4);
            this.sceEsprOn2Unknown[1] = sceEsprOn2Value.substring(4, 6);
            this.sceEsprOn2Unknown[2] = sceEsprOn2Value.substring(6, 8);
            this.sceEsprOn2Unknown[3] = sceEsprOn2Value.substring(8, 10);
            this.sceEsprOn2Unknown[4] = sceEsprOn2Value.substring(10, 12);
            this.sceEsprOn2Unknown[5] = sceEsprOn2Value.substring(12, 14);
            this.sceEsprOn2Unknown[6] = sceEsprOn2Value.substring(14, 16);
            this.sceEsprOn2Unknown[7] = sceEsprOn2Value.substring(16, 18);
            this.sceEsprOn2Unknown[8] = sceEsprOn2Value.substring(18, 20);
            this.sceEsprOn2Unknown[9] = sceEsprOn2Value.substring(20, 22);
            this.sceEsprOn2Unknown[10] = sceEsprOn2Value.substring(22, 24);
            this.sceEsprOn2Unknown[11] = sceEsprOn2Value.substring(24, 26);
            this.sceEsprOn2Unknown[12] = sceEsprOn2Value.substring(26, 28);
            this.sceEsprOn2Unknown[13] = sceEsprOn2Value.substring(28, 30);
            this.sceEsprOn2Unknown[14] = sceEsprOn2Value.substring(30, 32);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[0];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[1];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[2];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[3];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[4];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[5];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[6];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[7];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[8];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[9];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[10];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[11];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[12];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[13];
        newScdDataByteString = newScdDataByteString + sceEsprOn2Unknown[14];

        return newScdDataByteString;
    }
}
