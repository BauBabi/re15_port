package baubabi.reeditor.core.rdt.scd._31_op1F_nop1f;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Nop1f_nop1f_1F {
    //1 Byte

    /*
         Another nop
    */

    //1 byte
    private final String OPCODE = "1F";

    private final int size = 1;

    private final String displayName = "1F - third nop";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
