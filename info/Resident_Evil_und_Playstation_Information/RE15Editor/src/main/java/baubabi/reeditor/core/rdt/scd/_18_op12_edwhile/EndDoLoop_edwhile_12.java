package baubabi.reeditor.core.rdt.scd._18_op12_edwhile;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class EndDoLoop_edwhile_12 {
    //2 Byte

    /*
        End of DO block.

              Execute the block of instructions as long as the following Ck condition(s) are evaluated to TRUE.
    */

    //1 byte
    private final String OPCODE = "12";

    //1 byte - end do loop block
    private final String[] endDoLoopBlockLength = new String[1];

    private final int size = 2;

    private final String displayName = "12 - end do loop";

    public EndDoLoop_edwhile_12(String endDoLoopValue) {

        if(endDoLoopValue != null) {
            this.endDoLoopBlockLength[0] = endDoLoopValue.substring(2,4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + endDoLoopBlockLength[0];

        return newScdDataByteString;
    }
}
