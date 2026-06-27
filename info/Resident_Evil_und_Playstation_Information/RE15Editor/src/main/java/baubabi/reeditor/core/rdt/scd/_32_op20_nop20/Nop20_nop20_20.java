package baubabi.reeditor.core.rdt.scd._32_op20_nop20;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Nop20_nop20_20 {
    //1 Byte

    /*
         Another nop
    */

    //1 byte
    private final String OPCODE = "20";

    private final int size = 1;

    private final String displayName = "20 - fourth nop";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
