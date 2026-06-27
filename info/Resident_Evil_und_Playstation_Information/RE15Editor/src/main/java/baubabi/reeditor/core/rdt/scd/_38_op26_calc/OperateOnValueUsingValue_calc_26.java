package baubabi.reeditor.core.rdt.scd._38_op26_calc;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class OperateOnValueUsingValue_calc_26 {
    //6 Byte

    /*
        Operation on a variable, using immediate value.
    */

    //1 byte
    private final String OPCODE = "26"; /* 0x26 */

    //1 byte - Dummy of calc
    private final String[] operateOnValueUsingValueDummy = new String[1];

    //1 byte - operator:
    /* 0x00: ADD, 0x01:SUB, 0x02:MUL, 0x03:DIV */
    /* 0x04: MOD, 0x05:OR, 0x06:AND, 0x07:XOR */
    /* 0x08: NOT, 0x09:LSL, 0x0a:LSR, 0x0b:ASR */
    private final String[] operateOnValueUsingValueOperator = new String[1];

    //1 byte - Index of variable
    private final String[] operateOnValueUsingValueIndex = new String[1]; /* Index of variable */

    //2 byte - The parameter of operation
    private final String[] operateOnValueUsingValueOperationParameter = new String[2]; /* Parameter for operation */

    private final int size = 6;

    private final String displayName = "26 - operate on variable using value";

    public OperateOnValueUsingValue_calc_26(String operateOnValueUsingValueValue) {
        if(operateOnValueUsingValueValue != null) {
            this.operateOnValueUsingValueDummy[0] = operateOnValueUsingValueValue.substring(2,4);
            this.operateOnValueUsingValueOperator[0] = operateOnValueUsingValueValue.substring(4,6);
            this.operateOnValueUsingValueIndex[0] = operateOnValueUsingValueValue.substring(6,8);
            this.operateOnValueUsingValueOperationParameter[0] = operateOnValueUsingValueValue.substring(8,10);
            this.operateOnValueUsingValueOperationParameter[1] = operateOnValueUsingValueValue.substring(10,12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + operateOnValueUsingValueDummy[0];
        newScdDataByteString = newScdDataByteString + operateOnValueUsingValueOperator[0];
        newScdDataByteString = newScdDataByteString + operateOnValueUsingValueIndex[0];
        newScdDataByteString = newScdDataByteString + operateOnValueUsingValueOperationParameter[0];
        newScdDataByteString = newScdDataByteString + operateOnValueUsingValueOperationParameter[1];

        return newScdDataByteString;
    }
}
