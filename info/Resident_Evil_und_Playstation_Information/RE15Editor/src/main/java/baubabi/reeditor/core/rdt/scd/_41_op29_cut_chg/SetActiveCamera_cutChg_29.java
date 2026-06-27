package baubabi.reeditor.core.rdt.scd._41_op29_cut_chg;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetActiveCamera_cutChg_29 {
    //2 Byte

    /*
        Defines / Sets the active camera
    */

    //1 byte
    private final String OPCODE = "29"; /* 0x29 */

    //1 byte - Sets the active camera
    private final String[] setActiveCameraId = new String[1]; /* Set active camera */

    private final int size = 2;

    private final String displayName = "29 - cut change";

    public SetActiveCamera_cutChg_29(String setActiveCameraValue) {
        if(setActiveCameraValue != null) {
            this.setActiveCameraId[0] = setActiveCameraValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setActiveCameraId[0];

        return newScdDataByteString;
    }
}
