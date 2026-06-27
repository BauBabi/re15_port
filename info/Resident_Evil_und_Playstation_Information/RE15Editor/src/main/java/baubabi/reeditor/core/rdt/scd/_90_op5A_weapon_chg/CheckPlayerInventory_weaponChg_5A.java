package baubabi.reeditor.core.rdt.scd._90_op5A_weapon_chg;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class CheckPlayerInventory_weaponChg_5A {
    //2 Byte

    /*
        Checks player inventory.
    */

    //1 byte
    private final String OPCODE = "5A"; /* 0x5a */

    //1 byte - item id to check
    private final String[] checkPlayerInventoryItem = new String[1]; /* Item ID to check */

    private final int size = 2;

    private final String displayName = OPCODE + " - check player inventory";

    public CheckPlayerInventory_weaponChg_5A(String checkPlayerInventoryValue) {
        if(checkPlayerInventoryValue != null) {
            this.checkPlayerInventoryItem[0] = checkPlayerInventoryValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + checkPlayerInventoryItem[0];

        return newScdDataByteString;
    }
}
