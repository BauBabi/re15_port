package baubabi.reeditor.core.rdt.scd._92_op5C_sce_shake_on;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceShakeOn_sceShakeOn_5C {
    //3 Byte

    /*
        Don't know, Something with shake
    */

    //1 byte
    private final String OPCODE = "5C"; /* 0x5c */

    //2 byte - unknown value - an array of 2 with 1 byte each
    private final String[] sceShakeOnUnknown = new String[2]; /* Values are sign-extended to LONG before being processed */

    private final int size = 3;

    private final String displayName = OPCODE + " - sce shake on";

    public SceShakeOn_sceShakeOn_5C(String sceShakeOnValue) {
        if(sceShakeOnValue != null) {
            this.sceShakeOnUnknown[0] = sceShakeOnValue.substring(2, 4);
            this.sceShakeOnUnknown[1] = sceShakeOnValue.substring(4, 6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceShakeOnUnknown[0];
        newScdDataByteString = newScdDataByteString + sceShakeOnUnknown[1];

        return newScdDataByteString;
    }
}
