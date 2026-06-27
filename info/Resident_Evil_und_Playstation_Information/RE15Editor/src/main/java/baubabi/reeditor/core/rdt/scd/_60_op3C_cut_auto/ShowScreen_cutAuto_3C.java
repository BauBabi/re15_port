package baubabi.reeditor.core.rdt.scd._60_op3C_cut_auto;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ShowScreen_cutAuto_3C {
    //2 Byte

    /*
        Select which screen (inventory or map) to show when switching to status screen with instruction 0x42.
    */

    //1 byte
    private final String OPCODE = "3C"; /* 0x3c */

    //1 byte - the screen to show
    private final String[] showScreenScreen = new String[1]; /* 0: item screen, 1: map screen */

    private final int size = 2;

    private final String displayName = "3C - show screen (cut_auto)";

    public ShowScreen_cutAuto_3C(String showScreenValue) {
        if(showScreenValue != null) {
            this.showScreenScreen[0] = showScreenValue.substring(2,4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + showScreenScreen[0];

        return newScdDataByteString;
    }
}
