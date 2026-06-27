package baubabi.reeditor.core.rdt.scd._10_op0A_sleeping;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class BlockEventExecutionDuringSleep_sleeping_0A {
    //3 Byte

    /*
        Execute Sleep.

              Current event instructions will not be executed for the duration of sleep.
    */

    //1 byte
    private final String OPCODE = "0A";

    //1 byte - low count for sleep
    private final String[] blockEventExecutionDuringSleepCountLow = new String[1];

    //1 byte - high count for sleep
    private final String[] blockEventExecutionDuringSleepCountHigh = new String[1];

    private final int size = 3;

    private final String displayName = "0A - stop event during sleep duration";

    public BlockEventExecutionDuringSleep_sleeping_0A(String blockEventExecutionDuringSleepValue) {
        if(blockEventExecutionDuringSleepValue != null) {
            this.blockEventExecutionDuringSleepCountLow[0] = blockEventExecutionDuringSleepValue.substring(2,4);
            this.blockEventExecutionDuringSleepCountHigh[0] = blockEventExecutionDuringSleepValue.substring(4,6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + blockEventExecutionDuringSleepCountLow[0];
        newScdDataByteString = newScdDataByteString + blockEventExecutionDuringSleepCountHigh[0];

        return newScdDataByteString;
    }
}
