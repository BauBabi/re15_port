package baubabi.reeditor.core.rdt.scd._97_op61_cut_be_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class CameraSwitchSetting_cutBeSet_61 {
    //4 Byte

    /*
        Related to camera switches
    */

    //1 byte
    private final String OPCODE = "61"; /* 0x61 */

    //3 byte - unknown value - array of 3 with 1 byte each
    private final String[] cameraSwitchSetting = new String[3];

    private final int size = 4;

    private final String displayName = OPCODE + " - camera switch settings";

    public CameraSwitchSetting_cutBeSet_61(String cameraSwitchSettingValue) {
        if(cameraSwitchSettingValue != null) {
            this.cameraSwitchSetting[0] = cameraSwitchSettingValue.substring(2, 4);
            this.cameraSwitchSetting[1] = cameraSwitchSettingValue.substring(4, 6);
            this.cameraSwitchSetting[2] = cameraSwitchSettingValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + cameraSwitchSetting[0];
        newScdDataByteString = newScdDataByteString + cameraSwitchSetting[1];
        newScdDataByteString = newScdDataByteString + cameraSwitchSetting[2];

        return newScdDataByteString;
    }
}
