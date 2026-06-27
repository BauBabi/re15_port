package baubabi.reeditor.core.rdt.scd._39_op27_calc2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class OperateOnVariableUsingVariable_calc2_27 {
    //4 Byte

    /*
        Operation on a variable, using other variable as parameter.
    */

    //1 byte
    private final String OPCODE = "27"; /* 0x27 */

    //1 byte - operator:
    /* 0x00: ADD, 0x01:SUB, 0x02:MUL, 0x03:DIV */
    /* 0x04: MOD, 0x05:OR, 0x06:AND, 0x07:XOR */
    /* 0x08: NOT, 0x09:LSL, 0x0a:LSR, 0x0b:ASR */
    private final String[] operateOnVariableUsingVariableOperator = new String[1];

    //1 byte - Index of variable
    private final String[] operateOnVariableUsingVariableIndex = new String[1]; /* Index of variable */

    //1 byte - variable to use as operator parameter
    private final String[] operateOnVariableUsingVariableOperationParameter = new String[1]; /* Variable to use as parameter for operation */

    private final int size = 4;

    private final String displayName = "27 - operate on variable using variable";

    public OperateOnVariableUsingVariable_calc2_27(String operateOnVariableUsingVariableValue) {
        if(operateOnVariableUsingVariableValue != null) {
            this.operateOnVariableUsingVariableOperator[0] = operateOnVariableUsingVariableValue.substring(2,4);
            this.operateOnVariableUsingVariableIndex[0] = operateOnVariableUsingVariableValue.substring(4,6);
            this.operateOnVariableUsingVariableOperationParameter[0] = operateOnVariableUsingVariableValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + operateOnVariableUsingVariableOperator[0];
        newScdDataByteString = newScdDataByteString + operateOnVariableUsingVariableIndex[0];
        newScdDataByteString = newScdDataByteString + operateOnVariableUsingVariableOperationParameter[0];

        return newScdDataByteString;
    }
}
