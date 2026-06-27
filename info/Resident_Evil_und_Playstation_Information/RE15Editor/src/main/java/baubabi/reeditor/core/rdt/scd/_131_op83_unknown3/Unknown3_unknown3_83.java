package baubabi.reeditor.core.rdt.scd._131_op83_unknown3;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Unknown3_unknown3_83 {
    //1 Byte

    /*
        Unknown data
    */

    //1 byte
    private final String OPCODE = "83";

    private final int size = 1;

    private final String displayName = OPCODE + " - unknown 3";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
