package baubabi.reeditor.core.rdt.scd._49_op31_add_aspeed;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class AddSixWords_addAspeed_31 {
    //1 Byte

    /*
         Add 6 words together
    */

    //1 byte
    private final String OPCODE = "31";

    private final int size = 1;

    private final String displayName = "31 - add six words together";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
