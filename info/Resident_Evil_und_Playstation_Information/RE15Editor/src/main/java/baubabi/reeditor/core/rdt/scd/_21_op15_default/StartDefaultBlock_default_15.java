package baubabi.reeditor.core.rdt.scd._21_op15_default;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class StartDefaultBlock_default_15 {
    //2 Byte

    /*
        Start of DEFAULT block.

              Execute a block of instructions if none of preceding Case instructions matches.
    */

    //1 byte
    private final String OPCODE = "15";

    //1 byte - default block dummy
    private final String[] startDefaultBlockDummy = new String[1];

    private final int size = 2;

    private final String displayName = "15 - start default block";

    public StartDefaultBlock_default_15(String startDefaultBlockValue) {
        if(startDefaultBlockValue != null) {
            this.startDefaultBlockDummy[0] = startDefaultBlockValue.substring(2,4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + startDefaultBlockDummy[0];

        return newScdDataByteString;
    }
}
