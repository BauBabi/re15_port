package baubabi.reeditor.core.rdt.scd._95_op5F_xa_vol;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class VolumeSetting_xaVol_5F {
    //2 Byte

    /*
        Seems related to volume or balance, for music or sound fx
    */

    //1 byte
    private final String OPCODE = "5F"; /* 0x5f */

    //1 byte - unknown value
    private final String[] volumeSettingUnknown = new String[1];

    private final int size = 2;

    private final String displayName = OPCODE + " - volume setting";

    public VolumeSetting_xaVol_5F(String volumeSettingValue) {
        if(volumeSettingValue != null) {
            this.volumeSettingUnknown[0] = volumeSettingValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + volumeSettingUnknown[0];

        return newScdDataByteString;
    }
}
