package baubabi.reeditor.core.rdt.scd._121_op79_sce_line_end;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceLineEnd_sceLineEnd_79 {
    //1 Byte

    /*
        don't know the function
    */

    //1 byte
    private final String OPCODE = "79";

    private final int size = 1;

    private final String displayName = OPCODE + " - sce line end";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
