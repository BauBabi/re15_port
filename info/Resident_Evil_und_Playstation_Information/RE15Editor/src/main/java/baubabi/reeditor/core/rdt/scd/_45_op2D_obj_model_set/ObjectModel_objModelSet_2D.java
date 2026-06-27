package baubabi.reeditor.core.rdt.scd._45_op2D_obj_model_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ObjectModel_objModelSet_2D {
    //34 Byte

    /*
      Special item? Typewriter? Event trigger
      This object is added to the room objects list.
      Also sets the object
    */

    //1 byte
    private final String OPCODE = "2D"; /* 0x2d */

    //1 byte - Index of item in the array of the room list
    private final String[] objectModelIndex = new String[1]; /* Index of item in array of room objects list */

    //32 byte - Unknown value
    private final String[] objectModelUnknown = new String[32];

    private final int size = 34;

    private final String displayName = "2D - object model set";

    public ObjectModel_objModelSet_2D(String objectModelValue) {
        if(objectModelValue != null) {
            this.objectModelIndex[0] = objectModelValue.substring(2,4);
            this.objectModelUnknown[0] = objectModelValue.substring(4,6);
            this.objectModelUnknown[1] = objectModelValue.substring(6,8);
            this.objectModelUnknown[2] = objectModelValue.substring(8,10);
            this.objectModelUnknown[3] = objectModelValue.substring(10,12);
            this.objectModelUnknown[4] = objectModelValue.substring(12,14);
            this.objectModelUnknown[5] = objectModelValue.substring(14,16);
            this.objectModelUnknown[6] = objectModelValue.substring(16,18);
            this.objectModelUnknown[7] = objectModelValue.substring(18,20);
            this.objectModelUnknown[8] = objectModelValue.substring(20,22);
            this.objectModelUnknown[9] = objectModelValue.substring(22,24);
            this.objectModelUnknown[10] = objectModelValue.substring(24,26);
            this.objectModelUnknown[11] = objectModelValue.substring(26,28);
            this.objectModelUnknown[12] = objectModelValue.substring(28,30);
            this.objectModelUnknown[13] = objectModelValue.substring(30,32);
            this.objectModelUnknown[14] = objectModelValue.substring(32,34);
            this.objectModelUnknown[15] = objectModelValue.substring(34,36);
            this.objectModelUnknown[16] = objectModelValue.substring(36,38);
            this.objectModelUnknown[17] = objectModelValue.substring(38,40);
            this.objectModelUnknown[18] = objectModelValue.substring(40,42);
            this.objectModelUnknown[19] = objectModelValue.substring(42,44);
            this.objectModelUnknown[20] = objectModelValue.substring(44,46);
            this.objectModelUnknown[21] = objectModelValue.substring(46,48);
            this.objectModelUnknown[22] = objectModelValue.substring(48,50);
            this.objectModelUnknown[23] = objectModelValue.substring(50,52);
            this.objectModelUnknown[24] = objectModelValue.substring(52,54);
            this.objectModelUnknown[25] = objectModelValue.substring(54,56);
            this.objectModelUnknown[26] = objectModelValue.substring(56,58);
            this.objectModelUnknown[27] = objectModelValue.substring(58,60);
            this.objectModelUnknown[28] = objectModelValue.substring(60,62);
            this.objectModelUnknown[29] = objectModelValue.substring(62,64);
            this.objectModelUnknown[30] = objectModelValue.substring(64,66);
            this.objectModelUnknown[31] = objectModelValue.substring(66,68);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + objectModelIndex[0];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[0];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[1];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[2];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[3];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[4];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[5];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[6];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[7];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[8];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[9];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[10];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[11];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[12];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[13];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[14];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[15];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[16];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[17];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[18];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[19];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[20];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[21];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[22];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[23];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[24];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[25];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[26];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[27];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[28];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[29];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[30];
        newScdDataByteString = newScdDataByteString + objectModelUnknown[31];

        return newScdDataByteString;
    }
}
