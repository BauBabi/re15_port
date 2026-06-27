package baubabi.reeditor.core.rdt.scd._135_op87_poison_clr;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ClearPoisonStatus_poisonClr_87 {
    //1 Byte

    /*
        Clear player poisoned status.
    */

    //1 byte
    private final String OPCODE = "87";

    private final int size = 1;

    private final String displayName = OPCODE + " - clear poison status";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
