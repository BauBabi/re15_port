package baubabi.reeditor.core.rdt.scd._122_op7A_sce_parts_bomb;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ScePartsBomb_scePartsBomb_7A {
    //16 Byte

    /*
        Don't know the function of it
    */

    //1 byte
    private final String OPCODE = "7A";

    //15 byte - The Sce_parts_bomb data
    private final String[] scePartsBombData = new String[15];

    private final int size = 16;

    private final String displayName = OPCODE + " - sce parts bomb";

    public ScePartsBomb_scePartsBomb_7A(String scePartsBombValue) {
        if(scePartsBombValue != null) {
            this.scePartsBombData[0] = scePartsBombValue.substring(2, 4);
            this.scePartsBombData[1] = scePartsBombValue.substring(4, 6);
            this.scePartsBombData[2] = scePartsBombValue.substring(6, 8);
            this.scePartsBombData[3] = scePartsBombValue.substring(8, 10);
            this.scePartsBombData[4] = scePartsBombValue.substring(10, 12);
            this.scePartsBombData[5] = scePartsBombValue.substring(12, 14);
            this.scePartsBombData[6] = scePartsBombValue.substring(14, 16);
            this.scePartsBombData[7] = scePartsBombValue.substring(16, 18);
            this.scePartsBombData[8] = scePartsBombValue.substring(18, 20);
            this.scePartsBombData[9] = scePartsBombValue.substring(20, 22);
            this.scePartsBombData[10] = scePartsBombValue.substring(22, 24);
            this.scePartsBombData[11] = scePartsBombValue.substring(24, 26);
            this.scePartsBombData[12] = scePartsBombValue.substring(26, 28);
            this.scePartsBombData[13] = scePartsBombValue.substring(28, 30);
            this.scePartsBombData[14] = scePartsBombValue.substring(30, 32);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + scePartsBombData[0];
        newScdDataByteString = newScdDataByteString + scePartsBombData[1];
        newScdDataByteString = newScdDataByteString + scePartsBombData[2];
        newScdDataByteString = newScdDataByteString + scePartsBombData[3];
        newScdDataByteString = newScdDataByteString + scePartsBombData[4];
        newScdDataByteString = newScdDataByteString + scePartsBombData[5];
        newScdDataByteString = newScdDataByteString + scePartsBombData[6];
        newScdDataByteString = newScdDataByteString + scePartsBombData[7];
        newScdDataByteString = newScdDataByteString + scePartsBombData[8];
        newScdDataByteString = newScdDataByteString + scePartsBombData[9];
        newScdDataByteString = newScdDataByteString + scePartsBombData[10];
        newScdDataByteString = newScdDataByteString + scePartsBombData[11];
        newScdDataByteString = newScdDataByteString + scePartsBombData[12];
        newScdDataByteString = newScdDataByteString + scePartsBombData[13];
        newScdDataByteString = newScdDataByteString + scePartsBombData[14];

        return newScdDataByteString;
    }
}
