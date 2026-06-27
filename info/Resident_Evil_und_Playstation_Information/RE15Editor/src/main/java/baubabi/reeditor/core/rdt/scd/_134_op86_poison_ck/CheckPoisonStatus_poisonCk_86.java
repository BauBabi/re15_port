package baubabi.reeditor.core.rdt.scd._134_op86_poison_ck;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class CheckPoisonStatus_poisonCk_86 {
    //1 Byte

    /*
        Check player poisoned status.
    */

    //1 byte
    private final String OPCODE = "86";

    private final int size = 1;

    private final String displayName = OPCODE + " - check poison status";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
