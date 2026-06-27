package baubabi.reeditor.core.rdt.scd._09_op09_sleep;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Sleep_sleep_09 {
    //4 Byte

    /*
        Initialize Sleep.

              In the bytecode, you will find bytes 09 0a NN NN,
              and the script will only add 1 to current bytecode position for instruction 0x09,
              to get to instruction 0x0a, and process the sleep loop.
    */

    //1 byte
    private final String OPCODE = "09";

    //1 byte - dummy
    private final String[] sleepDummy = new String[1];

    //2 byte - the count for sleep
    private final String[] sleepCount = new String[2];

    private final int size = 4;

    private final String displayName = "09 - sleep";

    public Sleep_sleep_09(String sleepValue) {
        if(sleepValue != null) {
            this.sleepDummy[0] = sleepValue.substring(2,4);
            this.sleepCount[0] = sleepValue.substring(4,6);
            this.sleepCount[1] = sleepValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sleepDummy[0];
        newScdDataByteString = newScdDataByteString + sleepCount[0];
        newScdDataByteString = newScdDataByteString + sleepCount[1];

        return newScdDataByteString;
    }
}
