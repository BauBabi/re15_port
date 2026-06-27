package baubabi.reeditor.core.rdt.scd._02_op02_evt_next;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class EventWaitForPlayerInput_evtNext_02 {
    //1 Byte

    /*
         Wait for any player input (for example to answer a Yes/No question), before executing further script instructions.
    */

    //1 byte
    private final String OPCODE = "02";

    private final int size = 1;

    private final String displayName = "02 - event next (player input)";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
