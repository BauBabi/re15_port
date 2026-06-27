package baubabi.reeditor.core.rdt.scd._83_op53_sce_fade_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class FadeSettings_sceFadeSet_53 {
    //6 Byte

    /*
        Seems related to fade in/out settings.
    */

    //1 byte
    private final String OPCODE = "53"; /* 0x53 */

    //5 byte - unknown value
    private final String[] fadeSettingsUnknown = new String[5];

    private final int size = 6;

    private final String displayName = OPCODE + " - fade settings in / out";

    public FadeSettings_sceFadeSet_53(String fadeSettingsValue) {
        if(fadeSettingsValue != null) {
            this.fadeSettingsUnknown[0] = fadeSettingsValue.substring(2, 4);
            this.fadeSettingsUnknown[1] = fadeSettingsValue.substring(4, 6);
            this.fadeSettingsUnknown[2] = fadeSettingsValue.substring(6, 8);
            this.fadeSettingsUnknown[3] = fadeSettingsValue.substring(8, 10);
            this.fadeSettingsUnknown[4] = fadeSettingsValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + fadeSettingsUnknown[0];
        newScdDataByteString = newScdDataByteString + fadeSettingsUnknown[1];
        newScdDataByteString = newScdDataByteString + fadeSettingsUnknown[2];
        newScdDataByteString = newScdDataByteString + fadeSettingsUnknown[3];
        newScdDataByteString = newScdDataByteString + fadeSettingsUnknown[4];

        return newScdDataByteString;
    }
}
