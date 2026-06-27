package baubabi.reeditor.core.rdt.scd._108_op6C_rbj_reset;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class RbjReset_rbjReset_6C {
    //1 Byte

    /*
        Related to offset 22 of RDT file. Don't know the detail of it.
    */

    //1 byte
    private final String OPCODE = "6C";

    private final int size = 1;

    private final String displayName = OPCODE + " - rjb reset";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
