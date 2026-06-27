package baubabi.reeditor.core.rdt.scd._11_op0B_wsleep;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class InitializeGlobalSleep_wsleep_0B {
    //1 Byte

    /*
        MAYBE* Initialize global sleep.
    */

    //1 byte
    private final String OPCODE = "0B";

    private final int size = 1;

    private final String displayName = "0B - initialize global sleep";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
