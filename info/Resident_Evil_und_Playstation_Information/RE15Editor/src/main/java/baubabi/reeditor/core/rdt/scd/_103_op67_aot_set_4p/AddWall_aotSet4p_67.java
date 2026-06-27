package baubabi.reeditor.core.rdt.scd._103_op67_aot_set_4p;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class AddWall_aotSet4p_67 {
    //28 Byte

    /*
        The wall is added to the room objects list.
    */

    //1 byte
    private final String OPCODE = "67"; /* 0x67 */

    //1 byte - the index of the item in the room object list
    private final String[] addWallId = new String[1]; /* Index of item in array of room objects list */

    //4 byte - unknown value1 - an array of 2 with 2 byte each
    private final String[] addWallUnknown1 = new String[4];

    //16 byte - the x, y coordinates defining the zone player can not exit
    private final String[] addWallXYCoords = new String[16]; /* X,Y coordinates of 4 points defining a zone the player can not exit */

    //6 byte - unknown value2 - an array of 3 with 2 byte each
    private final String[] addWallUnknown2 = new String[6];

    private final int size = 28;

    private final String displayName = OPCODE + " - add wall to room obj list";

    public AddWall_aotSet4p_67(String addWallValue) {
        if(addWallValue != null) {
            this.addWallId[0] = addWallValue.substring(2, 4);
            this.addWallUnknown1[0] = addWallValue.substring(4, 6);
            this.addWallUnknown1[1] = addWallValue.substring(6, 8);
            this.addWallUnknown1[2] = addWallValue.substring(8, 10);
            this.addWallUnknown1[3] = addWallValue.substring(10, 12);
            this.addWallXYCoords[0] = addWallValue.substring(12, 14);
            this.addWallXYCoords[1] = addWallValue.substring(14, 16);
            this.addWallXYCoords[2] = addWallValue.substring(16, 18);
            this.addWallXYCoords[3] = addWallValue.substring(18, 20);
            this.addWallXYCoords[4] = addWallValue.substring(20, 22);
            this.addWallXYCoords[5] = addWallValue.substring(22, 24);
            this.addWallXYCoords[6] = addWallValue.substring(24, 26);
            this.addWallXYCoords[7] = addWallValue.substring(26, 28);
            this.addWallXYCoords[8] = addWallValue.substring(28, 30);
            this.addWallXYCoords[9] = addWallValue.substring(30, 32);
            this.addWallXYCoords[10] = addWallValue.substring(32, 34);
            this.addWallXYCoords[11] = addWallValue.substring(34, 36);
            this.addWallXYCoords[12] = addWallValue.substring(36, 38);
            this.addWallXYCoords[13] = addWallValue.substring(38, 40);
            this.addWallXYCoords[14] = addWallValue.substring(40, 42);
            this.addWallXYCoords[15] = addWallValue.substring(42, 44);
            this.addWallUnknown2[0] = addWallValue.substring(44, 46);
            this.addWallUnknown2[1] = addWallValue.substring(46, 48);
            this.addWallUnknown2[2] = addWallValue.substring(48, 50);
            this.addWallUnknown2[3] = addWallValue.substring(50, 52);
            this.addWallUnknown2[4] = addWallValue.substring(52, 54);
            this.addWallUnknown2[5] = addWallValue.substring(54, 56);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + addWallId[0];
        newScdDataByteString = newScdDataByteString + addWallUnknown1[0];
        newScdDataByteString = newScdDataByteString + addWallUnknown1[1];
        newScdDataByteString = newScdDataByteString + addWallUnknown1[2];
        newScdDataByteString = newScdDataByteString + addWallUnknown1[3];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[0];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[1];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[2];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[3];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[4];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[5];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[6];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[7];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[8];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[9];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[10];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[11];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[12];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[13];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[14];
        newScdDataByteString = newScdDataByteString + addWallXYCoords[15];
        newScdDataByteString = newScdDataByteString + addWallUnknown2[0];
        newScdDataByteString = newScdDataByteString + addWallUnknown2[1];
        newScdDataByteString = newScdDataByteString + addWallUnknown2[2];
        newScdDataByteString = newScdDataByteString + addWallUnknown2[3];
        newScdDataByteString = newScdDataByteString + addWallUnknown2[4];
        newScdDataByteString = newScdDataByteString + addWallUnknown2[5];

        return newScdDataByteString;
    }
}
