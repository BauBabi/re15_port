package baubabi.reeditor.core.rdt.scd._14_op0E_next;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class EndForLoop_next_0E {
    //2 Byte

    /*
        End of FOR block.
    */

    //1 byte
    private final String OPCODE = "0E";

    //1 byte - for dummy
    private final String[] EndForLoopDummy = new String[1];

    private final int size = 2;

    private final String displayName = "0E - end for loop";

    public EndForLoop_next_0E(String endForLoopValue) {
        if(endForLoopValue != null) {
            this.EndForLoopDummy[0] = endForLoopValue.substring(2,4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + EndForLoopDummy[0];

        return newScdDataByteString;
    }
}
