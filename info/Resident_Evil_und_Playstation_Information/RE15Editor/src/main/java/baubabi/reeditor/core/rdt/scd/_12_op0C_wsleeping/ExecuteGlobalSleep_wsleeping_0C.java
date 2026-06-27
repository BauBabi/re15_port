package baubabi.reeditor.core.rdt.scd._12_op0C_wsleeping;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ExecuteGlobalSleep_wsleeping_0C {
    //1 Byte

    /*
        MAYBE* Execute global sleep.

          All events instructions will not be executed for the duration of sleep.
    */

    //1 byte
    private final String OPCODE = "0C";

    private final int size = 1;

    private final String displayName = "0C - global sleep for duration";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
