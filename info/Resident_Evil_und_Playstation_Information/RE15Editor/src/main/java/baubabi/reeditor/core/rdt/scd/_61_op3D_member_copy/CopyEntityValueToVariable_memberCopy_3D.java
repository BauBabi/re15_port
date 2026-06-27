package baubabi.reeditor.core.rdt.scd._61_op3D_member_copy;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class CopyEntityValueToVariable_memberCopy_3D {
    //3 Byte

    /*
        Copy a given value for current entity to varw variable.
    */

    //1 byte
    private final String OPCODE = "3D"; /* 0x3d */

    //1 byte - the variable to copy the value to
    private final String[] copyEntityValueToVariableVarw = new String[1]; /* Variable to use */

    //1 byte - the value to read for current entity
    private final String[] copyEntityValueToVariableId = new String[1]; /* Value to read for current entity */

    private final int size = 3;

    private final String displayName = "3D - copy given value for current entity";

    public CopyEntityValueToVariable_memberCopy_3D(String copyEntityValueToVariableValue) {
        if(copyEntityValueToVariableValue != null) {
            this.copyEntityValueToVariableVarw[0] = copyEntityValueToVariableValue.substring(2,4);
            this.copyEntityValueToVariableId[0] = copyEntityValueToVariableValue.substring(4,6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + copyEntityValueToVariableVarw[0];
        newScdDataByteString = newScdDataByteString + copyEntityValueToVariableId[0];

        return newScdDataByteString;
    }
}
