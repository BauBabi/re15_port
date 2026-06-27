package baubabi.reeditor.core.rdt.scd._17_op11_do;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class StartDoLoop_do_11 {
    //4 Byte

    /*
        Start of DO block.

              Execute a block of instructions (condition at end of block).
    */

    //1 byte
    private final String OPCODE = "11";

    //1 byte - Do loop dummy
    private final String[] startDoLoopDummy = new String[1];

    //2 byte - do loop block length
    private final String[] startDoLoopBlockLength = new String[2];

    private final int size = 4;

    private final String displayName = "11 - do loop";

    public StartDoLoop_do_11(String startDoLoopValue) {
        if(startDoLoopValue != null) {
            this.startDoLoopDummy[0] = startDoLoopValue.substring(2,4);
            this.startDoLoopBlockLength[0] = startDoLoopValue.substring(4,6);
            this.startDoLoopBlockLength[1] = startDoLoopValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + startDoLoopDummy[0];
        newScdDataByteString = newScdDataByteString + startDoLoopBlockLength[0];
        newScdDataByteString = newScdDataByteString + startDoLoopBlockLength[1];

        return newScdDataByteString;
    }
}
