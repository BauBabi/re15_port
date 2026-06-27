package baubabi.reeditor.core.rdt.scd._139_op8B_nop8b;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Nop8b_nop8b_8B {
    //6 Byte

    /*
        Another nop
    */

    //1 byte
    private final String OPCODE = "8B";

    //5 byte - the nop data
    private final String[] nop8bData = new String[5];

    private final int size = 6;

    private final String displayName = OPCODE + " - sixth nop";

    public Nop8b_nop8b_8B(String nop8bValue) {
        if(nop8bValue != null) {
            this.nop8bData[0] = nop8bValue.substring(2, 4);
            this.nop8bData[1] = nop8bValue.substring(4, 6);
            this.nop8bData[2] = nop8bValue.substring(6, 8);
            this.nop8bData[3] = nop8bValue.substring(8, 10);
            this.nop8bData[4] = nop8bValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + nop8bData[0];
        newScdDataByteString = newScdDataByteString + nop8bData[1];
        newScdDataByteString = newScdDataByteString + nop8bData[2];
        newScdDataByteString = newScdDataByteString + nop8bData[3];
        newScdDataByteString = newScdDataByteString + nop8bData[4];

        return newScdDataByteString;
    }
}
