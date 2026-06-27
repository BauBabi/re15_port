package baubabi.reeditor.core.rdt.scd._30_op1E_nop1e;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Nop1e_nop1e_1E {
    //1 Byte

    /*
         Another nop
    */

    //1 byte
    private final String OPCODE = "1E";

    private final int size = 1;

    private final String displayName = "1E - second nop";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
