package baubabi.reeditor.core.rdt.scd._76_op4C_sce_espr_kill;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceEsprKill_sceEsprKill_4C {
    //5 Byte

    /*
        Something with Scene? Sprite and Kill? I don't know
    */

    //1 byte
    private final String OPCODE = "4C";

    //4 byte - unknown data for it
    private final String[] sceEsprKillUnknown = new String[4];

    private final int size = 5;

    private final String displayName = OPCODE + " - sce espr kill";

    public SceEsprKill_sceEsprKill_4C(String sceEsprKillValue) {
        if(sceEsprKillValue != null) {
            this.sceEsprKillUnknown[0] = sceEsprKillValue.substring(2, 4);
            this.sceEsprKillUnknown[1] = sceEsprKillValue.substring(4, 6);
            this.sceEsprKillUnknown[2] = sceEsprKillValue.substring(6, 8);
            this.sceEsprKillUnknown[3] = sceEsprKillValue.substring(8, 10);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceEsprKillUnknown[0];
        newScdDataByteString = newScdDataByteString + sceEsprKillUnknown[1];
        newScdDataByteString = newScdDataByteString + sceEsprKillUnknown[2];
        newScdDataByteString = newScdDataByteString + sceEsprKillUnknown[3];

        return newScdDataByteString;
    }
}
