package baubabi.reeditor.core.rdt.scd._84_op54_sce_espr_control;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceEspControl_sceEsprControl_54 {
    //8 Byte

    /*
        Something with sprite and 3d, don't know
    */

    //1 byte
    private final String OPCODE = "54"; /* 0x54 */

    //1 byte - Dummy for the sprite
    private final String[] sceEsprControlDummy = new String[1];

    //7 byte - unknown data - array of 10 with 2 byte each
    private final String[] sceEsprControlUnknown = new String[4];

    private final int size = 6;

    private final String displayName = OPCODE + " - sce espr control";

    public SceEspControl_sceEsprControl_54(String sceEspr3dOnValue) {
        if(sceEspr3dOnValue != null) {
            this.sceEsprControlDummy[0] = sceEspr3dOnValue.substring(2, 4);
            this.sceEsprControlUnknown[0] = sceEspr3dOnValue.substring(4, 6);
            this.sceEsprControlUnknown[1] = sceEspr3dOnValue.substring(6, 8);
            this.sceEsprControlUnknown[2] = sceEspr3dOnValue.substring(8, 10);
            this.sceEsprControlUnknown[3] = sceEspr3dOnValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceEsprControlDummy[0];
        newScdDataByteString = newScdDataByteString + sceEsprControlUnknown[0];
        newScdDataByteString = newScdDataByteString + sceEsprControlUnknown[1];
        newScdDataByteString = newScdDataByteString + sceEsprControlUnknown[2];
        newScdDataByteString = newScdDataByteString + sceEsprControlUnknown[3];

        return newScdDataByteString;
    }
}
