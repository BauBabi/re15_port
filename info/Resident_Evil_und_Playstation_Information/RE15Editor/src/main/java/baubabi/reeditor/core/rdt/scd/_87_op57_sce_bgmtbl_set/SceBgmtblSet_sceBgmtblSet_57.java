package baubabi.reeditor.core.rdt.scd._87_op57_sce_bgmtbl_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceBgmtblSet_sceBgmtblSet_57 {
    //8 Byte

    /*
        I don't know, could be background music table
    */

    //1 byte
    private final String OPCODE = "57"; /* 0x57 */

    //1 byte - some dummy value
    private final String[] sceBgmtblSetDummy = new String[1];

    //6 byte - unknown value - array of 3 with 2 byte each
    private final String[] sceBgmtblSetUnknown = new String[6];

    private final int size = 8;

    private final String displayName = OPCODE + " - sce bgm tbl set";

    public SceBgmtblSet_sceBgmtblSet_57(String sceBgmtblSetValue) {
        if(sceBgmtblSetValue != null) {
            this.sceBgmtblSetDummy[0] = sceBgmtblSetValue.substring(2, 4);
            this.sceBgmtblSetUnknown[0] = sceBgmtblSetValue.substring(4, 6);
            this.sceBgmtblSetUnknown[1] = sceBgmtblSetValue.substring(6, 8);
            this.sceBgmtblSetUnknown[2] = sceBgmtblSetValue.substring(8, 10);
            this.sceBgmtblSetUnknown[3] = sceBgmtblSetValue.substring(10, 12);
            this.sceBgmtblSetUnknown[4] = sceBgmtblSetValue.substring(12, 14);
            this.sceBgmtblSetUnknown[5] = sceBgmtblSetValue.substring(14, 16);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceBgmtblSetDummy[0];
        newScdDataByteString = newScdDataByteString + sceBgmtblSetUnknown[0];
        newScdDataByteString = newScdDataByteString + sceBgmtblSetUnknown[1];
        newScdDataByteString = newScdDataByteString + sceBgmtblSetUnknown[2];
        newScdDataByteString = newScdDataByteString + sceBgmtblSetUnknown[3];
        newScdDataByteString = newScdDataByteString + sceBgmtblSetUnknown[4];
        newScdDataByteString = newScdDataByteString + sceBgmtblSetUnknown[5];

        return newScdDataByteString;
    }
}
