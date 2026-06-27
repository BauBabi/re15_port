package baubabi.reeditor.core.rdt.scd._123_op7B_sce_parts_down;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ScePartsDown_scePartsDown_7B {
    //16 Byte

    /*
        Don't know the function of it
    */

    //1 byte
    private final String OPCODE = "7B";

    //15 byte - The Sce_parts_down data
    private final String[] scePartsDownData = new String[15];

    private final int size = 16;

    private final String displayName = OPCODE + " - sce parts down";

    public ScePartsDown_scePartsDown_7B(String scePartsDownValue) {
        if(scePartsDownValue != null) {
            this.scePartsDownData[0] = scePartsDownValue.substring(2, 4);
            this.scePartsDownData[1] = scePartsDownValue.substring(4, 6);
            this.scePartsDownData[2] = scePartsDownValue.substring(6, 8);
            this.scePartsDownData[3] = scePartsDownValue.substring(8, 10);
            this.scePartsDownData[4] = scePartsDownValue.substring(10, 12);
            this.scePartsDownData[5] = scePartsDownValue.substring(12, 14);
            this.scePartsDownData[6] = scePartsDownValue.substring(14, 16);
            this.scePartsDownData[7] = scePartsDownValue.substring(16, 18);
            this.scePartsDownData[8] = scePartsDownValue.substring(18, 20);
            this.scePartsDownData[9] = scePartsDownValue.substring(20, 22);
            this.scePartsDownData[10] = scePartsDownValue.substring(22, 24);
            this.scePartsDownData[11] = scePartsDownValue.substring(24, 26);
            this.scePartsDownData[12] = scePartsDownValue.substring(26, 28);
            this.scePartsDownData[13] = scePartsDownValue.substring(28, 30);
            this.scePartsDownData[14] = scePartsDownValue.substring(30, 32);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + scePartsDownData[0];
        newScdDataByteString = newScdDataByteString + scePartsDownData[1];
        newScdDataByteString = newScdDataByteString + scePartsDownData[2];
        newScdDataByteString = newScdDataByteString + scePartsDownData[3];
        newScdDataByteString = newScdDataByteString + scePartsDownData[4];
        newScdDataByteString = newScdDataByteString + scePartsDownData[5];
        newScdDataByteString = newScdDataByteString + scePartsDownData[6];
        newScdDataByteString = newScdDataByteString + scePartsDownData[7];
        newScdDataByteString = newScdDataByteString + scePartsDownData[8];
        newScdDataByteString = newScdDataByteString + scePartsDownData[9];
        newScdDataByteString = newScdDataByteString + scePartsDownData[10];
        newScdDataByteString = newScdDataByteString + scePartsDownData[11];
        newScdDataByteString = newScdDataByteString + scePartsDownData[12];
        newScdDataByteString = newScdDataByteString + scePartsDownData[13];
        newScdDataByteString = newScdDataByteString + scePartsDownData[14];

        return newScdDataByteString;
    }
}
