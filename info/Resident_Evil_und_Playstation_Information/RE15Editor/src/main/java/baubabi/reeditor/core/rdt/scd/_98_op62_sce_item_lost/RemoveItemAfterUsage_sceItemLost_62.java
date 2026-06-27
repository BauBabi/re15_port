package baubabi.reeditor.core.rdt.scd._98_op62_sce_item_lost;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class RemoveItemAfterUsage_sceItemLost_62 {
    //2 Byte

    /*
        Remove item from player inventory, after usage.
    */

    //1 byte
    private final String OPCODE = "62"; /* 0x62 */

    //1 byte - The item id to remove
    private final String[] removeItemAfterUsageItem = new String[1]; /* Item ID to remove */

    private final int size = 2;

    private final String displayName = OPCODE + " - remove item after usage";

    public RemoveItemAfterUsage_sceItemLost_62(String removeItemAfterUsageValue) {
        if(removeItemAfterUsageValue != null) {
            this.removeItemAfterUsageItem[0] = removeItemAfterUsageValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + removeItemAfterUsageItem[0];

        return newScdDataByteString;
    }
}
