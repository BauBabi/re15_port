package baubabi.reeditor.core.rdt.scd._101_op65_sce_espr_kill2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceEsprKill2_sceEsprKill2_65 {
    //2 Byte

    /*
        Don't know - remove sprites?
    */

    //1 byte
    private final String OPCODE = "65";	/* 0x65 */

    //1 byte - unknown value
    private final String[] sceEsprKill2Unknown = new String[1];

    private final int size = 2;

    private final String displayName = OPCODE + " - sce espr kill 2";

    public SceEsprKill2_sceEsprKill2_65(String sceEsprKill2Value) {
        if(sceEsprKill2Value != null) {
            this.sceEsprKill2Unknown[0] = sceEsprKill2Value.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceEsprKill2Unknown[0];

        return newScdDataByteString;
    }
}
