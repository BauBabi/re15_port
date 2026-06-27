package baubabi.reeditor.core.rdt.scd._66_op42_plc_ret;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ChangeToStatusScreen_plcRet_42 {
    //1 Byte

    /*
        Switch to status screen (after having picked up an item, or used an item).
    */

    //1 byte
    private final String OPCODE = "42";

    private final int size = 1;

    private final String displayName = "42 - change to status screen";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
