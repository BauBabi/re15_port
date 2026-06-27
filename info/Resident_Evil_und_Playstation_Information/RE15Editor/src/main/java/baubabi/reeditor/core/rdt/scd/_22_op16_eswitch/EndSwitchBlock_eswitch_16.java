package baubabi.reeditor.core.rdt.scd._22_op16_eswitch;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class EndSwitchBlock_eswitch_16 {
    //2 Byte

    /*
        End of SWITCH block.
    */

    //1 byte
    private final String OPCODE = "16";

    //1 byte - end switch block dummy
    private final String[] endSwitchBlockDummy = new String[1];

    private final int size = 2;

    private final String displayName = "16 - end switch block";

    public EndSwitchBlock_eswitch_16(String endSwitchBlockValue) {
        if(endSwitchBlockValue != null) {
            this.endSwitchBlockDummy[0] = endSwitchBlockValue.substring(2,4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + endSwitchBlockDummy[0];

        return newScdDataByteString;
    }
}
