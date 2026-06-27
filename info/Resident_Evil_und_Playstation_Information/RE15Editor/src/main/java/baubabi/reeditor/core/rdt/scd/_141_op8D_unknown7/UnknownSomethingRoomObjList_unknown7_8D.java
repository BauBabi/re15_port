package baubabi.reeditor.core.rdt.scd._141_op8D_unknown7;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class UnknownSomethingRoomObjList_unknown7_8D {
    //24 Byte

    /*
        Related to room object list
    */

    //1 byte
    private final String OPCODE = "8D";

    //23 byte - the data related to room object list
    private final String[] unknownSomethingRoomObjListData = new String[23];

    private final int size = 24;

    private final String displayName = OPCODE + " - unknown 7";

    public UnknownSomethingRoomObjList_unknown7_8D(String unknownSomethingRoomObjListValue) {
        if(unknownSomethingRoomObjListValue != null) {
            this.unknownSomethingRoomObjListData[0] = unknownSomethingRoomObjListValue.substring(2, 4);
            this.unknownSomethingRoomObjListData[1] = unknownSomethingRoomObjListValue.substring(4, 6);
            this.unknownSomethingRoomObjListData[2] = unknownSomethingRoomObjListValue.substring(6, 8);
            this.unknownSomethingRoomObjListData[3] = unknownSomethingRoomObjListValue.substring(8, 10);
            this.unknownSomethingRoomObjListData[4] = unknownSomethingRoomObjListValue.substring(10, 12);
            this.unknownSomethingRoomObjListData[5] = unknownSomethingRoomObjListValue.substring(12, 14);
            this.unknownSomethingRoomObjListData[6] = unknownSomethingRoomObjListValue.substring(14, 16);
            this.unknownSomethingRoomObjListData[7] = unknownSomethingRoomObjListValue.substring(16, 18);
            this.unknownSomethingRoomObjListData[8] = unknownSomethingRoomObjListValue.substring(18, 20);
            this.unknownSomethingRoomObjListData[9] = unknownSomethingRoomObjListValue.substring(20, 22);
            this.unknownSomethingRoomObjListData[10] = unknownSomethingRoomObjListValue.substring(22, 24);
            this.unknownSomethingRoomObjListData[11] = unknownSomethingRoomObjListValue.substring(24, 26);
            this.unknownSomethingRoomObjListData[12] = unknownSomethingRoomObjListValue.substring(26, 28);
            this.unknownSomethingRoomObjListData[13] = unknownSomethingRoomObjListValue.substring(28, 30);
            this.unknownSomethingRoomObjListData[14] = unknownSomethingRoomObjListValue.substring(30, 32);
            this.unknownSomethingRoomObjListData[15] = unknownSomethingRoomObjListValue.substring(32, 34);
            this.unknownSomethingRoomObjListData[16] = unknownSomethingRoomObjListValue.substring(34, 36);
            this.unknownSomethingRoomObjListData[17] = unknownSomethingRoomObjListValue.substring(36, 38);
            this.unknownSomethingRoomObjListData[18] = unknownSomethingRoomObjListValue.substring(38, 40);
            this.unknownSomethingRoomObjListData[19] = unknownSomethingRoomObjListValue.substring(40, 42);
            this.unknownSomethingRoomObjListData[20] = unknownSomethingRoomObjListValue.substring(42, 44);
            this.unknownSomethingRoomObjListData[21] = unknownSomethingRoomObjListValue.substring(44, 46);
            this.unknownSomethingRoomObjListData[22] = unknownSomethingRoomObjListValue.substring(46, 48);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[0];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[1];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[2];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[3];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[4];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[5];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[6];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[7];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[8];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[9];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[10];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[11];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[12];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[13];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[14];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[15];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[16];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[17];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[18];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[19];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[20];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[21];
        newScdDataByteString = newScdDataByteString + unknownSomethingRoomObjListData[22];

        return newScdDataByteString;
    }
}
