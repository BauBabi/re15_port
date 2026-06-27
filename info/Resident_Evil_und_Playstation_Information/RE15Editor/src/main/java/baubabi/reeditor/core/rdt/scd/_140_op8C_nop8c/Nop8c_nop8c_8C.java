package baubabi.reeditor.core.rdt.scd._140_op8C_nop8c;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Nop8c_nop8c_8C {
    //8 Byte

    /*
        Another nop
    */

    //1 byte
    private final String OPCODE = "8C";

    //7 byte - the nop data
    private final String[] nop8cData = new String[7];

    private final int size = 8;

    private final String displayName = OPCODE + " - seventh nop";

    public Nop8c_nop8c_8C(String nop8cValue) {
        if(nop8cValue != null) {
            this.nop8cData[0] = nop8cValue.substring(2, 4);
            this.nop8cData[1] = nop8cValue.substring(4, 6);
            this.nop8cData[2] = nop8cValue.substring(6, 8);
            this.nop8cData[3] = nop8cValue.substring(8, 10);
            this.nop8cData[4] = nop8cValue.substring(10, 12);
            this.nop8cData[5] = nop8cValue.substring(12, 14);
            this.nop8cData[6] = nop8cValue.substring(14, 16);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + nop8cData[0];
        newScdDataByteString = newScdDataByteString + nop8cData[1];
        newScdDataByteString = newScdDataByteString + nop8cData[2];
        newScdDataByteString = newScdDataByteString + nop8cData[3];
        newScdDataByteString = newScdDataByteString + nop8cData[4];
        newScdDataByteString = newScdDataByteString + nop8cData[5];
        newScdDataByteString = newScdDataByteString + nop8cData[6];

        return newScdDataByteString;
    }
}
