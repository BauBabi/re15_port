package baubabi.reeditor.core.rdt.scd._142_op8E_unknown8;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class UnknownSomethingWithEnemies_unknown8_8E {
    //24 Byte

    /*
        Related to enemies
    */

    //1 byte
    private final String OPCODE = "8E";

    //23 byte - the data related to enemies
    private final String[] unknownSomethingWithEnemiesData = new String[23];

    private final int size = 24;

    private final String displayName = OPCODE + " - unknown 8";

    public UnknownSomethingWithEnemies_unknown8_8E(String unknownSomethingWithEnemiesValue) {
        if(unknownSomethingWithEnemiesValue != null) {
            this.unknownSomethingWithEnemiesData[0] = unknownSomethingWithEnemiesValue.substring(2, 4);
            this.unknownSomethingWithEnemiesData[1] = unknownSomethingWithEnemiesValue.substring(4, 6);
            this.unknownSomethingWithEnemiesData[2] = unknownSomethingWithEnemiesValue.substring(6, 8);
            this.unknownSomethingWithEnemiesData[3] = unknownSomethingWithEnemiesValue.substring(8, 10);
            this.unknownSomethingWithEnemiesData[4] = unknownSomethingWithEnemiesValue.substring(10, 12);
            this.unknownSomethingWithEnemiesData[5] = unknownSomethingWithEnemiesValue.substring(12, 14);
            this.unknownSomethingWithEnemiesData[6] = unknownSomethingWithEnemiesValue.substring(14, 16);
            this.unknownSomethingWithEnemiesData[7] = unknownSomethingWithEnemiesValue.substring(16, 18);
            this.unknownSomethingWithEnemiesData[8] = unknownSomethingWithEnemiesValue.substring(18, 20);
            this.unknownSomethingWithEnemiesData[9] = unknownSomethingWithEnemiesValue.substring(20, 22);
            this.unknownSomethingWithEnemiesData[10] = unknownSomethingWithEnemiesValue.substring(22, 24);
            this.unknownSomethingWithEnemiesData[11] = unknownSomethingWithEnemiesValue.substring(24, 26);
            this.unknownSomethingWithEnemiesData[12] = unknownSomethingWithEnemiesValue.substring(26, 28);
            this.unknownSomethingWithEnemiesData[13] = unknownSomethingWithEnemiesValue.substring(28, 30);
            this.unknownSomethingWithEnemiesData[14] = unknownSomethingWithEnemiesValue.substring(30, 32);
            this.unknownSomethingWithEnemiesData[15] = unknownSomethingWithEnemiesValue.substring(32, 34);
            this.unknownSomethingWithEnemiesData[16] = unknownSomethingWithEnemiesValue.substring(34, 36);
            this.unknownSomethingWithEnemiesData[17] = unknownSomethingWithEnemiesValue.substring(36, 38);
            this.unknownSomethingWithEnemiesData[18] = unknownSomethingWithEnemiesValue.substring(38, 40);
            this.unknownSomethingWithEnemiesData[19] = unknownSomethingWithEnemiesValue.substring(40, 42);
            this.unknownSomethingWithEnemiesData[20] = unknownSomethingWithEnemiesValue.substring(42, 44);
            this.unknownSomethingWithEnemiesData[21] = unknownSomethingWithEnemiesValue.substring(44, 46);
            this.unknownSomethingWithEnemiesData[22] = unknownSomethingWithEnemiesValue.substring(46, 48);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[0];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[1];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[2];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[3];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[4];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[5];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[6];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[7];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[8];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[9];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[10];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[11];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[12];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[13];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[14];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[15];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[16];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[17];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[18];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[19];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[20];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[21];
        newScdDataByteString = newScdDataByteString + unknownSomethingWithEnemiesData[22];

        return newScdDataByteString;
    }
}
