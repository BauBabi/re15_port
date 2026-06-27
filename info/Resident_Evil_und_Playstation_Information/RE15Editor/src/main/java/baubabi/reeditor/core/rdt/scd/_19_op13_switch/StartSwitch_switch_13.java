package baubabi.reeditor.core.rdt.scd._19_op13_switch;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class StartSwitch_switch_13 {
    //4 Byte

    /*
        Start of SWITCH block.

          Execute a block of instructions depending on the value of 'varw' variable.
    */

    //1 byte
    private final String OPCODE = "13";

    //1 byte - the varw variable which decide block execution
    private final String[] startSwitchVarw = new String[1];

    //2 byte - block length
    private final String[] startSwitchBlockLength = new String[2];

    private final int size = 4;

    private final String displayName = "13 - start switch";

    public StartSwitch_switch_13(String startSwitchValue) {
        if(startSwitchValue != null) {
            this.startSwitchVarw[0] = startSwitchValue.substring(2,4);
            this.startSwitchBlockLength[0] = startSwitchValue.substring(4,6);
            this.startSwitchBlockLength[1] = startSwitchValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + startSwitchVarw[0];
        newScdDataByteString = newScdDataByteString + startSwitchBlockLength[0];
        newScdDataByteString = newScdDataByteString + startSwitchBlockLength[1];

        return newScdDataByteString;
    }
}
