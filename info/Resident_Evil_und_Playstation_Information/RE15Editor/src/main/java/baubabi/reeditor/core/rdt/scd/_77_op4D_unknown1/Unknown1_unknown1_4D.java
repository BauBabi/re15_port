package baubabi.reeditor.core.rdt.scd._77_op4D_unknown1;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Unknown1_unknown1_4D {
    //22 Byte

    /*
        Fully Unknown Data
    */

    //1 byte
    private final String OPCODE = "4D";

    //21 byte the unknown data for the unknown type
    private final String[] unknown1Unknown = new String[21];

    private final int size = 22;

    private final String displayName = OPCODE + " - unknown 1";

    public Unknown1_unknown1_4D(String unknown1Value) {
        if(unknown1Value != null) {
            this.unknown1Unknown[0] = unknown1Value.substring(2, 4);
            this.unknown1Unknown[1] = unknown1Value.substring(4, 6);
            this.unknown1Unknown[2] = unknown1Value.substring(6, 8);
            this.unknown1Unknown[3] = unknown1Value.substring(8, 10);
            this.unknown1Unknown[4] = unknown1Value.substring(10, 12);
            this.unknown1Unknown[5] = unknown1Value.substring(12, 14);
            this.unknown1Unknown[6] = unknown1Value.substring(14, 16);
            this.unknown1Unknown[7] = unknown1Value.substring(16, 18);
            this.unknown1Unknown[8] = unknown1Value.substring(18, 20);
            this.unknown1Unknown[9] = unknown1Value.substring(20, 22);
            this.unknown1Unknown[10] = unknown1Value.substring(22, 24);
            this.unknown1Unknown[11] = unknown1Value.substring(24, 26);
            this.unknown1Unknown[12] = unknown1Value.substring(26, 28);
            this.unknown1Unknown[13] = unknown1Value.substring(28, 30);
            this.unknown1Unknown[14] = unknown1Value.substring(30, 32);
            this.unknown1Unknown[15] = unknown1Value.substring(32, 34);
            this.unknown1Unknown[16] = unknown1Value.substring(34, 36);
            this.unknown1Unknown[17] = unknown1Value.substring(36, 38);
            this.unknown1Unknown[18] = unknown1Value.substring(38, 40);
            this.unknown1Unknown[19] = unknown1Value.substring(40, 42);
            this.unknown1Unknown[20] = unknown1Value.substring(42, 44);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + unknown1Unknown[0];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[1];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[2];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[3];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[4];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[5];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[6];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[7];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[8];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[9];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[10];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[11];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[12];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[13];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[14];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[15];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[16];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[17];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[18];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[19];
        newScdDataByteString = newScdDataByteString + unknown1Unknown[20];

        return newScdDataByteString;
    }
}
