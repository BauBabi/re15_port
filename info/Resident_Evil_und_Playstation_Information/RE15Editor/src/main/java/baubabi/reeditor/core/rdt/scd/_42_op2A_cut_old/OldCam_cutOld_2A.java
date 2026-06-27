package baubabi.reeditor.core.rdt.scd._42_op2A_cut_old;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class OldCam_cutOld_2A {
    //1 Byte

    /*
        Maybe The old camera?
    */

    //1 byte
    private final String OPCODE = "2A";

    private final int size = 1;

    private final String displayName = "2A - cut old";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
