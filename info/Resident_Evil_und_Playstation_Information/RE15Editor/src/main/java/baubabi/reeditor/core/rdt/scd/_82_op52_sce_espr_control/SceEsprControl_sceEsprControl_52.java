package baubabi.reeditor.core.rdt.scd._82_op52_sce_espr_control;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceEsprControl_sceEsprControl_52 {
    //6 Byte

    /*
        Something - I don't know - Sprite control?
    */

    //1 byte
    private final String OPCODE = "52"; /* 0x52 */

    //5 byte - unknown value
    private final String[] sceEsprControlUnknown = new String[5];

    private final int size = 6;

    private final String displayName = OPCODE + " - sce espr control";

    public SceEsprControl_sceEsprControl_52(String sceEsprControlValue) {
        if(sceEsprControlValue != null) {
            this.sceEsprControlUnknown[0] = sceEsprControlValue.substring(2, 4);
            this.sceEsprControlUnknown[1] = sceEsprControlValue.substring(4, 6);
            this.sceEsprControlUnknown[2] = sceEsprControlValue.substring(6, 8);
            this.sceEsprControlUnknown[3] = sceEsprControlValue.substring(8, 10);
            this.sceEsprControlUnknown[4] = sceEsprControlValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceEsprControlUnknown[0];
        newScdDataByteString = newScdDataByteString + sceEsprControlUnknown[1];
        newScdDataByteString = newScdDataByteString + sceEsprControlUnknown[2];
        newScdDataByteString = newScdDataByteString + sceEsprControlUnknown[3];
        newScdDataByteString = newScdDataByteString + sceEsprControlUnknown[4];

        return newScdDataByteString;
    }
}
