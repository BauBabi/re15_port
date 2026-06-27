package baubabi.reeditor.core.rdt.scd._06_op06_ifel_ck;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class IfCase_ifelCk_06 {
    //4 Byte

    /*
        IF block instruction start.

          Any following Ck instruction evaluated to FALSE will end this block.
    */

    //1 byte
    private final String OPCODE = "06";

    //1 byte - dummy
    private final String[] ifCaseDummy = new String[1];

    //2 Byte - Block length of if block
    private final String[] ifCaseBlockLength = new String[2];

    private final int size = 4;

    private final String displayName = "06 - if else case";

    public IfCase_ifelCk_06(String ifCaseValue) {
        if(ifCaseValue != null) {
            this.ifCaseDummy[0] = ifCaseValue.substring(2, 4);
            this.ifCaseBlockLength[0] = ifCaseValue.substring(4, 6);
            this.ifCaseBlockLength[1] = ifCaseValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + ifCaseDummy[0];
        newScdDataByteString = newScdDataByteString + ifCaseBlockLength[0];
        newScdDataByteString = newScdDataByteString + ifCaseBlockLength[1];

        return newScdDataByteString;
    }
}
