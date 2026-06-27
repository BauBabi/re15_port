package baubabi.reeditor.core.rdt.scd._00_op00_nop;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class NoOperation_nop_00 {
    //1 Byte

    /*
        NO Operation.
    */

    //1 byte
    private final String OPCODE = "00";

    private final int size = 1;

    private final String displayName = "00 - no operation";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
