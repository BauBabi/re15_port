package baubabi.reeditor.core.rdt.scd._36_op24_save;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetVariableValue_save_24 {
    //4 Byte

    /*
        Set a value for variable.
    */

    //1 byte
    private final String OPCODE = "24"; /* 0x24 */

    //1 byte - Index of the source variable
    private final String[] setVariableValueIndex = new String[1]; /* Index of source variable */

    //2 byte - The new value of the variable
    private final String[] setVariableValueValue = new String[2]; /* New value of variable */

    private final int size = 4;

    private final String displayName = "24 - set variable value";

    public SetVariableValue_save_24(String setVariableValueValue) {
        if(setVariableValueValue != null) {
            this.setVariableValueIndex[0] = setVariableValueValue.substring(2,4);
            this.setVariableValueValue[0] = setVariableValueValue.substring(4,6);
            this.setVariableValueValue[1] = setVariableValueValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setVariableValueIndex[0];
        newScdDataByteString = newScdDataByteString + setVariableValueValue[0];
        newScdDataByteString = newScdDataByteString + setVariableValueValue[1];

        return newScdDataByteString;
    }
}
