package baubabi.reeditor.core.rdt.scd._118_op76_sce_item_get;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class AddItemToInventory_sceItemGet_76 {
    //3 Byte

    /*
        Don't know the function of it
    */

    //1 byte
    private final String OPCODE = "76"; /* 0x76 */

    //1 byte - the number of the item file to load
    private final String[] addItemToInventoryId = new String[1]; /* Number of gitemXX.adt file to load */

    //1 byte - the item amount
    private final String[] addItemToInventoryAmount = new String[1]; /* Amount */

    private final int size = 3;

    private final String displayName = OPCODE + " - add item to inventory";

    public AddItemToInventory_sceItemGet_76(String addItemToInventoryValue) {
        if(addItemToInventoryValue != null) {
            this.addItemToInventoryId[0] = addItemToInventoryValue.substring(2, 4);
            this.addItemToInventoryAmount[0] = addItemToInventoryValue.substring(4, 6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + addItemToInventoryId[0];
        newScdDataByteString = newScdDataByteString + addItemToInventoryAmount[0];

        return newScdDataByteString;
    }
}
