package baubabi.reeditor.core.rdt.scd._105_op69_item_aot_set_4p;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class AddItem_itemAotSet4p_69 {
    //30 Byte

    /*
        This Item is added to the room objects list.
    */

    //1 byte
    private final String OPCODE = "69"; /* 0x69 */

    //1 byte - the index of the item in the room object list
    private final String[] addItemId = new String[1]; /* Index of item in array of room objects list */

    //28 byte - unknown value - an array of 14 with 2 byte each
    private final String[] addItemUnknown = new String[28];

    private final int size = 30;

    private final String displayName = OPCODE + " - add item to room obj list";

    public AddItem_itemAotSet4p_69(String addItemValue) {
        if(addItemValue != null) {
            this.addItemId[0] = addItemValue.substring(2, 4);
            this.addItemUnknown[0] = addItemValue.substring(4, 6);
            this.addItemUnknown[1] = addItemValue.substring(6, 8);
            this.addItemUnknown[2] = addItemValue.substring(8, 10);
            this.addItemUnknown[3] = addItemValue.substring(10, 12);
            this.addItemUnknown[4] = addItemValue.substring(12, 14);
            this.addItemUnknown[5] = addItemValue.substring(14, 16);
            this.addItemUnknown[6] = addItemValue.substring(16, 18);
            this.addItemUnknown[7] = addItemValue.substring(18, 20);
            this.addItemUnknown[8] = addItemValue.substring(20, 22);
            this.addItemUnknown[9] = addItemValue.substring(22, 24);
            this.addItemUnknown[10] = addItemValue.substring(24, 26);
            this.addItemUnknown[11] = addItemValue.substring(26, 28);
            this.addItemUnknown[12] = addItemValue.substring(28, 30);
            this.addItemUnknown[13] = addItemValue.substring(30, 32);
            this.addItemUnknown[14] = addItemValue.substring(32, 34);
            this.addItemUnknown[15] = addItemValue.substring(34, 36);
            this.addItemUnknown[16] = addItemValue.substring(36, 38);
            this.addItemUnknown[17] = addItemValue.substring(38, 40);
            this.addItemUnknown[18] = addItemValue.substring(40, 42);
            this.addItemUnknown[19] = addItemValue.substring(42, 44);
            this.addItemUnknown[20] = addItemValue.substring(44, 46);
            this.addItemUnknown[21] = addItemValue.substring(46, 48);
            this.addItemUnknown[22] = addItemValue.substring(48, 50);
            this.addItemUnknown[23] = addItemValue.substring(50, 52);
            this.addItemUnknown[24] = addItemValue.substring(52, 54);
            this.addItemUnknown[25] = addItemValue.substring(54, 56);
            this.addItemUnknown[26] = addItemValue.substring(56, 58);
            this.addItemUnknown[27] = addItemValue.substring(58, 60);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + addItemId[0];
        newScdDataByteString = newScdDataByteString + addItemUnknown[0];
        newScdDataByteString = newScdDataByteString + addItemUnknown[1];
        newScdDataByteString = newScdDataByteString + addItemUnknown[2];
        newScdDataByteString = newScdDataByteString + addItemUnknown[3];
        newScdDataByteString = newScdDataByteString + addItemUnknown[4];
        newScdDataByteString = newScdDataByteString + addItemUnknown[5];
        newScdDataByteString = newScdDataByteString + addItemUnknown[6];
        newScdDataByteString = newScdDataByteString + addItemUnknown[7];
        newScdDataByteString = newScdDataByteString + addItemUnknown[8];
        newScdDataByteString = newScdDataByteString + addItemUnknown[9];
        newScdDataByteString = newScdDataByteString + addItemUnknown[10];
        newScdDataByteString = newScdDataByteString + addItemUnknown[11];
        newScdDataByteString = newScdDataByteString + addItemUnknown[12];
        newScdDataByteString = newScdDataByteString + addItemUnknown[13];
        newScdDataByteString = newScdDataByteString + addItemUnknown[14];
        newScdDataByteString = newScdDataByteString + addItemUnknown[15];
        newScdDataByteString = newScdDataByteString + addItemUnknown[16];
        newScdDataByteString = newScdDataByteString + addItemUnknown[17];
        newScdDataByteString = newScdDataByteString + addItemUnknown[18];
        newScdDataByteString = newScdDataByteString + addItemUnknown[19];
        newScdDataByteString = newScdDataByteString + addItemUnknown[20];
        newScdDataByteString = newScdDataByteString + addItemUnknown[21];
        newScdDataByteString = newScdDataByteString + addItemUnknown[22];
        newScdDataByteString = newScdDataByteString + addItemUnknown[23];
        newScdDataByteString = newScdDataByteString + addItemUnknown[24];
        newScdDataByteString = newScdDataByteString + addItemUnknown[25];
        newScdDataByteString = newScdDataByteString + addItemUnknown[26];
        newScdDataByteString = newScdDataByteString + addItemUnknown[27];

        return newScdDataByteString;
    }
}
