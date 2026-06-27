package baubabi.reeditor.core.rdt.scd._81_op51_sce_key_ck_renamed;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceKeyCk_sceKeyCk_51 {
    //4 Byte

    /*
        Something with key - don't know
    */

    //1 byte
    private final String OPCODE = "51"; /* 0x51 */

    //1 byte - unknown value 1
    private final String[] sceKeyCkUnknown1 = new String[1];

    //2 byte - unknown value 2
    private final String[] sceKeyCkUnknown2 = new String[2];

    private final int size = 4;

    private final String displayName = OPCODE + " - sce key ck";

    public SceKeyCk_sceKeyCk_51(String sceKeyCkValue) {
        if(sceKeyCkValue != null) {
            this.sceKeyCkUnknown1[0] = sceKeyCkValue.substring(2, 4);
            this.sceKeyCkUnknown2[0] = sceKeyCkValue.substring(4, 6);
            this.sceKeyCkUnknown2[1] = sceKeyCkValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceKeyCkUnknown1[0];
        newScdDataByteString = newScdDataByteString + sceKeyCkUnknown2[0];
        newScdDataByteString = newScdDataByteString + sceKeyCkUnknown2[1];

        return newScdDataByteString;
    }
}
