package baubabi.reeditor.core.rdt.scd._29_op1D_work_copy;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ChangeInstruction_workCopy_1D {
    //4 Byte

    /*
        Change instruction by writing value inside script.

          'varw' is the variable we want to write at offset 'offset' (relative to start of next instruction).
          'size' tells if we write a byte or a word.
    */

    //1 byte
    private final String OPCODE = "1D";

    //1 byte - the variable to write at offset
    private final String[] changeInstructionVarw = new String[1];

    //1 byte - the offset to write the data to
    private final String[] changeInstructionOffset = new String[1];

    //1 byte - the size to write - byte or word
    private final String[] changeInstructionSize = new String[1];

    private final int size = 4;

    private final String displayName = "1D - change instruction by value";

    public ChangeInstruction_workCopy_1D(String changeInstructionValue) {
        if(changeInstructionValue != null) {
            this.changeInstructionVarw[0] = changeInstructionValue.substring(2,4);
            this.changeInstructionOffset[0] = changeInstructionValue.substring(4,6);
            this.changeInstructionSize[0] = changeInstructionValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + changeInstructionVarw[0];
        newScdDataByteString = newScdDataByteString + changeInstructionOffset[0];
        newScdDataByteString = newScdDataByteString + changeInstructionSize[0];

        return newScdDataByteString;
    }
}
