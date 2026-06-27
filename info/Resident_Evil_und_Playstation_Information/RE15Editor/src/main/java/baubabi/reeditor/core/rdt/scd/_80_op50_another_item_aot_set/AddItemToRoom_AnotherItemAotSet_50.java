package baubabi.reeditor.core.rdt.scd._80_op50_another_item_aot_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class AddItemToRoom_AnotherItemAotSet_50 {
    //22 Byte

    /*
        The item is added to the room object list. Pickable object? Typewriter?
    */

    //1 byte
    private final String OPCODE = "50"; /* 0x50 */

    //1 byte - The id of the item
    private final String[] addItemToRoomId = new String[1]; /* Index of item in array of room objects list */

    //4 byte - unknown value1
    private final String[] addItemToRoomUnknown1 = new String[4];

    //2 byte - the position X of item
    private final String[] addItemToRoomPositionX = new String[2];

    //2 byte - the position Y of item
    private final String[] addItemToRoomPositionY = new String[2];

    //2 byte - the Width of item
    private final String[] addItemToRoomWidth = new String[2];

    //2 byte - the Height of item
    private final String[] addItemToRoomHeight = new String[2];

    //2 byte - the item type
    private final String[] addItemToRoomType = new String[2]; /* 0x07: flower? typewrite ribbon? 0x14: ammo for gun */

    //2 byte - the item amount
    /* For ammo: number of bullets/shells/etc */
    /* For a key: number of times it can be used before the 'Discard key?' message */
    private final String[] addItemToRoomAmount = new String[2];

    //2 byte - The bit number 0x08 in array
    private final String[] addItemToRoomTypeArray08Idx = new String[2]; /* Bit number in array 0x08 (see end of section) */

    //2 byte - unknown value2
    private final String[] addItemToRoomUnknown2 = new String[2];

    private final int size = 22;

    private final String displayName = OPCODE + " - add another item to room";

    public AddItemToRoom_AnotherItemAotSet_50(String addItemToRoomValue) {
        if(addItemToRoomValue != null) {
            this.addItemToRoomId[0] = addItemToRoomValue.substring(2, 4);
            this.addItemToRoomUnknown1[0] = addItemToRoomValue.substring(4, 6);
            this.addItemToRoomUnknown1[1] = addItemToRoomValue.substring(6, 8);
            this.addItemToRoomUnknown1[2] = addItemToRoomValue.substring(8, 10);
            this.addItemToRoomUnknown1[3] = addItemToRoomValue.substring(10, 12);
            this.addItemToRoomPositionX[0] = addItemToRoomValue.substring(12, 14);
            this.addItemToRoomPositionX[1] = addItemToRoomValue.substring(14, 16);
            this.addItemToRoomPositionY[0] = addItemToRoomValue.substring(16, 18);
            this.addItemToRoomPositionY[1] = addItemToRoomValue.substring(18, 20);
            this.addItemToRoomWidth[0] = addItemToRoomValue.substring(20, 22);
            this.addItemToRoomWidth[1] = addItemToRoomValue.substring(22, 24);
            this.addItemToRoomHeight[0] = addItemToRoomValue.substring(24, 26);
            this.addItemToRoomHeight[1] = addItemToRoomValue.substring(26, 28);
            this.addItemToRoomType[0] = addItemToRoomValue.substring(28, 30);
            this.addItemToRoomType[1] = addItemToRoomValue.substring(30, 32);
            this.addItemToRoomAmount[0] = addItemToRoomValue.substring(32, 34);
            this.addItemToRoomAmount[1] = addItemToRoomValue.substring(34, 36);
            this.addItemToRoomTypeArray08Idx[0] = addItemToRoomValue.substring(36, 38);
            this.addItemToRoomTypeArray08Idx[1] = addItemToRoomValue.substring(38, 40);
            this.addItemToRoomUnknown2[0] = addItemToRoomValue.substring(40, 42);
            this.addItemToRoomUnknown2[1] = addItemToRoomValue.substring(42, 44);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + addItemToRoomId[0];
        newScdDataByteString = newScdDataByteString + addItemToRoomUnknown1[0];
        newScdDataByteString = newScdDataByteString + addItemToRoomUnknown1[1];
        newScdDataByteString = newScdDataByteString + addItemToRoomUnknown1[2];
        newScdDataByteString = newScdDataByteString + addItemToRoomUnknown1[3];
        newScdDataByteString = newScdDataByteString + addItemToRoomPositionX[0];
        newScdDataByteString = newScdDataByteString + addItemToRoomPositionX[1];
        newScdDataByteString = newScdDataByteString + addItemToRoomPositionY[0];
        newScdDataByteString = newScdDataByteString + addItemToRoomPositionY[1];
        newScdDataByteString = newScdDataByteString + addItemToRoomWidth[0];
        newScdDataByteString = newScdDataByteString + addItemToRoomWidth[1];
        newScdDataByteString = newScdDataByteString + addItemToRoomHeight[0];
        newScdDataByteString = newScdDataByteString + addItemToRoomHeight[1];
        newScdDataByteString = newScdDataByteString + addItemToRoomType[0];
        newScdDataByteString = newScdDataByteString + addItemToRoomType[1];
        newScdDataByteString = newScdDataByteString + addItemToRoomAmount[0];
        newScdDataByteString = newScdDataByteString + addItemToRoomAmount[1];
        newScdDataByteString = newScdDataByteString + addItemToRoomTypeArray08Idx[0];
        newScdDataByteString = newScdDataByteString + addItemToRoomTypeArray08Idx[1];
        newScdDataByteString = newScdDataByteString + addItemToRoomUnknown2[0];
        newScdDataByteString = newScdDataByteString + addItemToRoomUnknown2[1];

        return newScdDataByteString;
    }
}
