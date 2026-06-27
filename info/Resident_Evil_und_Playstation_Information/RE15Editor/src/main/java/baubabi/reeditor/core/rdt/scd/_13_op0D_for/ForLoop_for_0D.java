package baubabi.reeditor.core.rdt.scd._13_op0D_for;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ForLoop_for_0D {
    //6 Byte

    /*
 	    Initialize FOR loop.

        Execute a block of instructions 'count' times.

    */

    //1 byte
    private final String OPCODE = "0D";

    //1 byte - dummy for for loop
    private final String[] forLoopDummy = new String[1];

    //2 byte - the block length
    private final String[] forLoopBlockLength = new String[2];

    //2 byte - the loop count
    private final String[] forLoopCount = new String[2];

    private final int size = 6;

    private final String displayName = "0D - for loop";

    public ForLoop_for_0D(String forLoopValue) {
        if(forLoopValue != null) {
            this.forLoopDummy[0] = forLoopValue.substring(2,4);
            this.forLoopBlockLength[0] = forLoopValue.substring(4,6);
            this.forLoopBlockLength[1] = forLoopValue.substring(6,8);
            this.forLoopCount[0] = forLoopValue.substring(8,10);
            this.forLoopCount[1] = forLoopValue.substring(10,12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + forLoopDummy[0];
        newScdDataByteString = newScdDataByteString + forLoopBlockLength[0];
        newScdDataByteString = newScdDataByteString + forLoopBlockLength[1];
        newScdDataByteString = newScdDataByteString + forLoopCount[0];
        newScdDataByteString = newScdDataByteString + forLoopCount[1];

        return newScdDataByteString;
    }
}
