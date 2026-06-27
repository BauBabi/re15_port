package baubabi.reeditor.core.rdt.scd._08_op08_endif;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class EndIfCase_endif_08 {
    //1 Byte

    /*
        IF block instruction end.

              Exit current IF instruction block.
    */

    //1 byte
    private final String OPCODE = "08";

    private final int size = 1;

    private final String displayName = "08 - end if case";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
