package baubabi.reeditor.core.rdt.scd._94_op5E_keep_item_ck;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class CheckPlayerItem_keepItemCk_5E {
    //2 Byte

    /*
        Check whether player has item in its inventory or not.
    */

    //1 byte
    private final String OPCODE = "5E"; /* 0x5e */

    //1 byte - the item id to check
    private final String[] checkPlayerItemItem = new String[1]; /* Item ID to check */

    private final int size = 2;

    private final String displayName = OPCODE + " - check player for item";

    public CheckPlayerItem_keepItemCk_5E(String checkPlayerItemValue) {
        if(checkPlayerItemValue != null) {
            this.checkPlayerItemItem[0] = checkPlayerItemValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + checkPlayerItemItem[0];

        return newScdDataByteString;
    }
}
