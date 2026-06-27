package baubabi.reeditor.core.rdt.scd._137_op89_unknown6;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Unknown6_unknown6_89 {
    //1 Byte

    /*
        Unknown data
    */

    //1 byte
    private final String OPCODE = "89";

    private final int size = 1;

    private final String displayName = OPCODE + " - unknown 6";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
