package baubabi.reeditor.core.rdt.scd._07_op07_else_ck;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ElseCase_elseCk_07 {
    //4 Byte

    /*
        ELSE block instruction start.

          Block to execute if precedent IF block must be stopped if some condition evaluated to FALSE.
          Note: it's either IF/ENDIF, or IF/ELSE; there is no IF/ELSE/ENDIF sequence.
    */

    //1 byte
    private final String OPCODE = "07";

    //1 byte - dummy
    private final String[] elseCaseDummy = new String[1];

    //2 byte - block length to execute
    private final String[] elseCaseBlockLength = new String[2];

    private final int size = 4;

    private final String displayName = "07 - else case";

    public ElseCase_elseCk_07(String elseCaseValue) {
        if(elseCaseValue != null) {
            this.elseCaseDummy[0] = elseCaseValue.substring(2, 4);
            this.elseCaseBlockLength[0] = elseCaseValue.substring(4, 6);
            this.elseCaseBlockLength[1] = elseCaseValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + elseCaseDummy[0];
        newScdDataByteString = newScdDataByteString + elseCaseBlockLength[0];
        newScdDataByteString = newScdDataByteString + elseCaseBlockLength[1];

        return newScdDataByteString;
    }
}
