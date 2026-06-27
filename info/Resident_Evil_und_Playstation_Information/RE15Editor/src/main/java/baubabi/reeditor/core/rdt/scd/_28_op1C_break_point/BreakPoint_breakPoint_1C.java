package baubabi.reeditor.core.rdt.scd._28_op1C_break_point;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class BreakPoint_breakPoint_1C {
    //1 Byte

    /*
         eventually break the for for2 loop?
    */

    //1 byte
    private final String OPCODE = "1C";

    private final int size = 1;

    private final String displayName = "1C - breakpoint for second loop";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
