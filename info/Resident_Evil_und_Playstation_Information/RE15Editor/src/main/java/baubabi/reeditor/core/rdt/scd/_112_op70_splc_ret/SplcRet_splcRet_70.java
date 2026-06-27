package baubabi.reeditor.core.rdt.scd._112_op70_splc_ret;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SplcRet_splcRet_70 {
    //1 Byte

    /*
        No idea what it is.
    */

    //1 byte
    private final String OPCODE = "70";

    private final int size = 1;

    private final String displayName = OPCODE + " - splc ret";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
