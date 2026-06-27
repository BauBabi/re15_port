package baubabi.reeditor.core.rdt.scd._113_op71_splc_sce;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SplcSce_splcSce_71 {
    //1 Byte

    /*
        No idea what it is.
    */

    //1 byte
    private final String OPCODE = "71";

    private final int size = 1;

    private final String displayName = OPCODE + " - splc sce";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
