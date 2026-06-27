package baubabi.reeditor.core.rdt.scd._15_op0F_while;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class WhileLoop_while_0F {
    //4 Byte

    /*
 	    Start of WHILE block. Execute a block of instructions unless a following Ck condition is evaluated to FALSE.
    */

    //1 byte
    private final String OPCODE = "0F";

    //1 byte - while dummy
    private final String[] whileLoopDummy = new String[1];

    //2 byte - while block length
    private final String[] whileLoopBlockLength = new String[2];

    private final int size = 4;

    private final String displayName = "0F - start while block";

    public WhileLoop_while_0F(String whileLoopValue) {
        if(whileLoopValue != null) {
            this.whileLoopDummy[0] = whileLoopValue.substring(2,4);
            this.whileLoopBlockLength[0] = whileLoopValue.substring(4,6);
            this.whileLoopBlockLength[1] = whileLoopValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + whileLoopDummy[0];
        newScdDataByteString = newScdDataByteString + whileLoopBlockLength[0];
        newScdDataByteString = newScdDataByteString + whileLoopBlockLength[1];

        return newScdDataByteString;
    }
}
