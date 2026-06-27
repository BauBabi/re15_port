package baubabi.reeditor.core.rdt.scd._25_op19_return;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ReturnGosub_return_19 {
    //2 Byte

    /*
        End a subroutine called with Gosub.
    */

    //1 byte
    private final String OPCODE = "19";

    //1 byte - the return dummy
    private final String[] returnGosubDummy = new String[1];

    private final int size = 2;

    private final String displayName = "19 - end sub";

    public ReturnGosub_return_19(String returnGosubValue) {
        if(returnGosubValue != null) {
            this.returnGosubDummy[0] = returnGosubValue.substring(2,4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + returnGosubDummy[0];

        return newScdDataByteString;
    }
}
