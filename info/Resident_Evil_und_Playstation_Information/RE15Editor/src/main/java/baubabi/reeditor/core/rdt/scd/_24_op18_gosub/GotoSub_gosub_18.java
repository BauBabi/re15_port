package baubabi.reeditor.core.rdt.scd._24_op18_gosub;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class GotoSub_gosub_18 {
    //2 Byte

    /*
        Execute a subroutine for this event.
    */

    //1 byte
    private final String OPCODE = "18";

    //1 byte - the sub event to go to
    private final String[] gotoSubEvent = new String[1];

    private final int size = 2;

    private final String displayName = "18 - goto sub";

    public GotoSub_gosub_18(String gotoSubValue) {
        if(gotoSubValue != null) {
            this.gotoSubEvent[0] = gotoSubValue.substring(2,4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + gotoSubEvent[0];

        return newScdDataByteString;
    }
}
