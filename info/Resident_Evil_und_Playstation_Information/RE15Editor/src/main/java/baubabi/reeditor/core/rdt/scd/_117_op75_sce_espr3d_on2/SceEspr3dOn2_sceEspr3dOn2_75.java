package baubabi.reeditor.core.rdt.scd._117_op75_sce_espr3d_on2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceEspr3dOn2_sceEspr3dOn2_75 {
    //22 Byte

    /*
        Something with sprite and 3d, don't know
    */

    //1 byte
    private final String OPCODE = "75";

    //21 byte - The Sce_espr3d_on2 data
    private final String[] sceEspr3dOn2Data = new String[21];

    private final int size = 22;

    private final String displayName = OPCODE + " - sce espr 3d on 2";

    public SceEspr3dOn2_sceEspr3dOn2_75(String sceEspr3dOn2Value) {
        if(sceEspr3dOn2Value != null) {
            this.sceEspr3dOn2Data[0] = sceEspr3dOn2Value.substring(2, 4);
            this.sceEspr3dOn2Data[1] = sceEspr3dOn2Value.substring(4, 6);
            this.sceEspr3dOn2Data[2] = sceEspr3dOn2Value.substring(6, 8);
            this.sceEspr3dOn2Data[3] = sceEspr3dOn2Value.substring(8, 10);
            this.sceEspr3dOn2Data[4] = sceEspr3dOn2Value.substring(10, 12);
            this.sceEspr3dOn2Data[5] = sceEspr3dOn2Value.substring(12, 14);
            this.sceEspr3dOn2Data[6] = sceEspr3dOn2Value.substring(14, 16);
            this.sceEspr3dOn2Data[7] = sceEspr3dOn2Value.substring(16, 18);
            this.sceEspr3dOn2Data[8] = sceEspr3dOn2Value.substring(18, 20);
            this.sceEspr3dOn2Data[9] = sceEspr3dOn2Value.substring(20, 22);
            this.sceEspr3dOn2Data[10] = sceEspr3dOn2Value.substring(22, 24);
            this.sceEspr3dOn2Data[11] = sceEspr3dOn2Value.substring(24, 26);
            this.sceEspr3dOn2Data[12] = sceEspr3dOn2Value.substring(26, 28);
            this.sceEspr3dOn2Data[13] = sceEspr3dOn2Value.substring(28, 30);
            this.sceEspr3dOn2Data[14] = sceEspr3dOn2Value.substring(30, 32);
            this.sceEspr3dOn2Data[15] = sceEspr3dOn2Value.substring(32, 34);
            this.sceEspr3dOn2Data[16] = sceEspr3dOn2Value.substring(34, 36);
            this.sceEspr3dOn2Data[17] = sceEspr3dOn2Value.substring(36, 38);
            this.sceEspr3dOn2Data[18] = sceEspr3dOn2Value.substring(38, 40);
            this.sceEspr3dOn2Data[19] = sceEspr3dOn2Value.substring(40, 42);
            this.sceEspr3dOn2Data[20] = sceEspr3dOn2Value.substring(42, 44);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[0];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[1];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[2];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[3];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[4];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[5];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[6];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[7];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[8];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[9];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[10];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[11];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[12];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[13];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[14];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[15];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[16];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[17];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[18];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[19];
        newScdDataByteString = newScdDataByteString + sceEspr3dOn2Data[20];

        return newScdDataByteString;
    }
}
