package baubabi.reeditor.core.rdt.scd._104_op68_door_aot_set_4p;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class AddDoor_doorAotSet4p_68 {
    //40 Byte

    /*
        This Door is added to the room objects list.
    */

    //1 byte
    private final String OPCODE = "68"; /* 0x68 */

    //1 byte - the index of the door in the room object list
    private final String[] addDoorId = new String[1]; /* Index of door in array of room objects list */

    //38 byte - unknown value - an array of 19 with 2 byte each
    private final String[] addDoorUnknown = new String[38];

    private final int size = 40;

    private final String displayName = OPCODE + " - add door to room obj list";

    public AddDoor_doorAotSet4p_68(String addDoorValue) {
        if(addDoorValue != null) {
            this.addDoorId[0] = addDoorValue.substring(2, 4);
            this.addDoorUnknown[0] = addDoorValue.substring(4, 6);
            this.addDoorUnknown[1] = addDoorValue.substring(6, 8);
            this.addDoorUnknown[2] = addDoorValue.substring(8, 10);
            this.addDoorUnknown[3] = addDoorValue.substring(10, 12);
            this.addDoorUnknown[4] = addDoorValue.substring(12, 14);
            this.addDoorUnknown[5] = addDoorValue.substring(14, 16);
            this.addDoorUnknown[6] = addDoorValue.substring(16, 18);
            this.addDoorUnknown[7] = addDoorValue.substring(18, 20);
            this.addDoorUnknown[8] = addDoorValue.substring(20, 22);
            this.addDoorUnknown[9] = addDoorValue.substring(22, 24);
            this.addDoorUnknown[10] = addDoorValue.substring(24, 26);
            this.addDoorUnknown[11] = addDoorValue.substring(26, 28);
            this.addDoorUnknown[12] = addDoorValue.substring(28, 30);
            this.addDoorUnknown[13] = addDoorValue.substring(30, 32);
            this.addDoorUnknown[14] = addDoorValue.substring(32, 34);
            this.addDoorUnknown[15] = addDoorValue.substring(34, 36);
            this.addDoorUnknown[16] = addDoorValue.substring(36, 38);
            this.addDoorUnknown[17] = addDoorValue.substring(38, 40);
            this.addDoorUnknown[18] = addDoorValue.substring(40, 42);
            this.addDoorUnknown[19] = addDoorValue.substring(42, 44);
            this.addDoorUnknown[20] = addDoorValue.substring(44, 46);
            this.addDoorUnknown[21] = addDoorValue.substring(46, 48);
            this.addDoorUnknown[22] = addDoorValue.substring(48, 50);
            this.addDoorUnknown[23] = addDoorValue.substring(50, 52);
            this.addDoorUnknown[24] = addDoorValue.substring(52, 54);
            this.addDoorUnknown[25] = addDoorValue.substring(54, 56);
            this.addDoorUnknown[26] = addDoorValue.substring(56, 58);
            this.addDoorUnknown[27] = addDoorValue.substring(58, 60);
            this.addDoorUnknown[28] = addDoorValue.substring(60, 62);
            this.addDoorUnknown[29] = addDoorValue.substring(62, 64);
            this.addDoorUnknown[30] = addDoorValue.substring(64, 66);
            this.addDoorUnknown[31] = addDoorValue.substring(66, 68);
            this.addDoorUnknown[32] = addDoorValue.substring(68, 70);
            this.addDoorUnknown[33] = addDoorValue.substring(70, 72);
            this.addDoorUnknown[34] = addDoorValue.substring(72, 74);
            this.addDoorUnknown[35] = addDoorValue.substring(74, 76);
            this.addDoorUnknown[36] = addDoorValue.substring(76, 78);
            this.addDoorUnknown[37] = addDoorValue.substring(78, 80);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + addDoorId[0];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[0];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[1];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[2];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[3];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[4];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[5];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[6];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[7];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[8];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[9];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[10];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[11];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[12];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[13];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[14];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[15];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[16];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[17];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[18];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[19];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[20];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[21];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[22];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[23];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[24];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[25];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[26];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[27];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[28];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[29];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[30];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[31];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[32];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[33];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[34];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[35];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[36];
        newScdDataByteString = newScdDataByteString + addDoorUnknown[37];

        return newScdDataByteString;
    }
}
