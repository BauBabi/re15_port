package baubabi.reeditor.core.rdt.scd._16_op10_ewhile;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class EndWhileLoop_ewhile_10 {
    //2 Byte

    /*
 	    End of WHILE block.
    */

    //1 byte
    private final String OPCODE = "10";

    //1 byte - end while loop dummy
    private final String[] endWhileLoopDummy = new String[1];

    private final int size = 2;

    private final String displayName = "10 - end while block";

    public EndWhileLoop_ewhile_10(String endWhileLoopValue) {
        if(endWhileLoopValue != null) {
            this.endWhileLoopDummy[0] = endWhileLoopValue.substring(2,4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + endWhileLoopDummy[0];

        return newScdDataByteString;
    }
}
