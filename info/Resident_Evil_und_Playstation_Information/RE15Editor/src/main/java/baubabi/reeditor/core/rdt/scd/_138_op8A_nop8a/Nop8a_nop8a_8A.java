package baubabi.reeditor.core.rdt.scd._138_op8A_nop8a;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Nop8a_nop8a_8A {
    //6 Byte

    /*
        Another nop
    */

    //1 byte
    private final String OPCODE = "8A";

    //5 byte - the nop data
    private final String[] nop8aData = new String[5];

    private final int size = 6;

    private final String displayName = OPCODE + " - fifth nop";
    public Nop8a_nop8a_8A(String nop8aValue) {
        if(nop8aValue != null) {
            this.nop8aData[0] = nop8aValue.substring(2, 4);
            this.nop8aData[1] = nop8aValue.substring(4, 6);
            this.nop8aData[2] = nop8aValue.substring(6, 8);
            this.nop8aData[3] = nop8aValue.substring(8, 10);
            this.nop8aData[4] = nop8aValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + nop8aData[0];
        newScdDataByteString = newScdDataByteString + nop8aData[1];
        newScdDataByteString = newScdDataByteString + nop8aData[2];
        newScdDataByteString = newScdDataByteString + nop8aData[3];
        newScdDataByteString = newScdDataByteString + nop8aData[4];

        return newScdDataByteString;
    }
}
