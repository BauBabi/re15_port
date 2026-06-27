package baubabi.reeditor.core.rdt.scd._136_op88_sce_item_ck_lost;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class CheckItemInInventoryAndRemove_sceItemCkLost_88 {
    //3 Byte

    /*
        Check if player has a given item, and remove it from inventory if this is the case (likely used for single usage items).
    */

    //1 byte
    private final String OPCODE = "88"; /* 0x88 */

    //1 byte - the item id to remove
    private final String[] checkItemInInventoryAndRemoveItem = new String[1]; /* Item ID to check */

    //1 byte - unknown value
    private final String[] checkItemInInventoryAndRemoveUnknown = new String[1];

    private final int size = 3;

    private final String displayName = OPCODE + " - check item in inventory and remove";

    public CheckItemInInventoryAndRemove_sceItemCkLost_88(String checkItemInInventoryAndRemoveValue) {
        if(checkItemInInventoryAndRemoveValue != null) {
            this.checkItemInInventoryAndRemoveItem[0] = checkItemInInventoryAndRemoveValue.substring(2, 4);
            this.checkItemInInventoryAndRemoveUnknown[0] = checkItemInInventoryAndRemoveValue.substring(4, 6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + checkItemInInventoryAndRemoveItem[0];
        newScdDataByteString = newScdDataByteString + checkItemInInventoryAndRemoveUnknown[0];

        return newScdDataByteString;
    }
}
