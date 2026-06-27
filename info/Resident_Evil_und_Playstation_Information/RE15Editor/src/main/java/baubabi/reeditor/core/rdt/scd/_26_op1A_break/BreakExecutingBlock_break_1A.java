package baubabi.reeditor.core.rdt.scd._26_op1A_break;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class BreakExecutingBlock_break_1A {
    //2 Byte

    /*
         Exit a currently executing block (For, Case, Do, etc).
    */

    //1 byte
    private final String OPCODE = "1A";

    //1 byte - break dummy
    private final String[] breakExecutingBlockDummy = new String[1];

    private final int size = 2;

    private final String displayName = "1A - exit execution block";

    public BreakExecutingBlock_break_1A(String breakExecutingBlockValue) {
        if(breakExecutingBlockValue != null) {
            this.breakExecutingBlockDummy[0] = breakExecutingBlockValue.substring(2,4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + breakExecutingBlockDummy[0];

        return newScdDataByteString;
    }
}
