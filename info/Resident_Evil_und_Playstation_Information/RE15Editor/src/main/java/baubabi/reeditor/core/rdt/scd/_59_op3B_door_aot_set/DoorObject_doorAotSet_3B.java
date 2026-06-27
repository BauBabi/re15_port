package baubabi.reeditor.core.rdt.scd._59_op3B_door_aot_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class DoorObject_doorAotSet_3B {
    //32 Byte

    /*
        The door is added to the room objects list.
    */

    //1 byte
    private final String OPCODE = "3B"; /* 0x3b */

    //1 byte - id of door in room object list
    private final String[] doorObjectId = new String[1]; /* Index of item in array of room objects list */

    //4 byte - unknown value 1 - Array with 2 Entries, of 2 bytes each
    private final String[] doorObjectUnknown1 = new String[4];

    //2 byte - door location
    private final String[] doorObjectDoorLocationX = new String[2]; /* Location of door */

    //2 byte - door location
    private final String[] doorObjectDoorLocationY = new String[2]; /* Location of door */

    //2 byte - door size
    private final String[] doorObjectDoorWidth = new String[2]; /* Size of door */

    //2 byte - door size
    private final String[] doorObjectDoorHeight = new String[2]; /* Size of door */

    //2 byte - Player position after entering door
    private final String[] doorObjectPlayerPositionAfterEnteringX = new String[2]; /* Position of player after door entered */

    //2 byte - Player position after entering door
    private final String[] doorObjectPlayerPositionAfterEnteringY = new String[2]; /* Position of player after door entered */

    //2 byte - Player position after entering door
    private final String[] doorObjectPlayerPositionAfterEnteringZ = new String[2]; /* Position of player after door entered */

    //2 byte - Player direction after entering door
    private final String[] doorObjectPlayerPositionAfterEnteringNextDirection = new String[2]; /* Direction of player after door entered */

    //1 byte - stage after entering door
    private final String[] doorObjectStageAfterEntering = new String[1]; /* Stage after door entered */

    //1 byte - room after entering door
    private final String[] doorObjectRoomAfterEntering = new String[1]; /* Room after door entered */

    //1 byte - camera after entering door
    private final String[] doorObjectCameraAfterEntering = new String[1]; /* Camera after door entered */

    //1 byte - unknown value 2
    private final String[] doorObjectUnknown2 = new String[1];

    //1 byte - the door type
    private final String[] doorObjectDoorType = new String[1];

    //1 byte - the door lock
    private final String[] doorObjectDoorLock = new String[1];

    //1 byte - unknown value 3
    private final String[] doorObjectUnknown3 = new String[1];

    //1 byte - is door locked
    private final String[] doorObjectDoorLocked = new String[1];

    //1 byte - the door key
    private final String[] doorObjectDoorKey = new String[1];

    //1 byte - unknown value 4
    private final String[] doorObjectUnknown4 = new String[1];

    private final int size = 32;

    private final String displayName = "3B - door object (door_aot_set)";

    public DoorObject_doorAotSet_3B(String doorObjectValue) {
        if(doorObjectValue != null) {
            this.doorObjectId[0] = doorObjectValue.substring(2,4);
            this.doorObjectUnknown1[0] = doorObjectValue.substring(4,6);
            this.doorObjectUnknown1[1] = doorObjectValue.substring(6,8);
            this.doorObjectUnknown1[2] = doorObjectValue.substring(8,10);
            this.doorObjectUnknown1[3] = doorObjectValue.substring(10,12);
            this.doorObjectDoorLocationX[0] = doorObjectValue.substring(12,14);
            this.doorObjectDoorLocationX[1] = doorObjectValue.substring(14,16);
            this.doorObjectDoorLocationY[0] = doorObjectValue.substring(16,18);
            this.doorObjectDoorLocationY[1] = doorObjectValue.substring(18,20);
            this.doorObjectDoorWidth[0] = doorObjectValue.substring(20,22);
            this.doorObjectDoorWidth[1] = doorObjectValue.substring(22,24);
            this.doorObjectDoorHeight[0] = doorObjectValue.substring(24,26);
            this.doorObjectDoorHeight[1] = doorObjectValue.substring(26,28);
            this.doorObjectPlayerPositionAfterEnteringX[0] = doorObjectValue.substring(28,30);
            this.doorObjectPlayerPositionAfterEnteringX[1] = doorObjectValue.substring(30,32);
            this.doorObjectPlayerPositionAfterEnteringY[0] = doorObjectValue.substring(32,34);
            this.doorObjectPlayerPositionAfterEnteringY[1] = doorObjectValue.substring(34,36);
            this.doorObjectPlayerPositionAfterEnteringZ[0] = doorObjectValue.substring(36,38);
            this.doorObjectPlayerPositionAfterEnteringZ[1] = doorObjectValue.substring(38,40);
            this.doorObjectPlayerPositionAfterEnteringNextDirection[0] = doorObjectValue.substring(40,42);
            this.doorObjectPlayerPositionAfterEnteringNextDirection[1] = doorObjectValue.substring(42,44);
            this.doorObjectStageAfterEntering[0] = doorObjectValue.substring(44,46);
            this.doorObjectRoomAfterEntering[0] = doorObjectValue.substring(46,48);
            this.doorObjectCameraAfterEntering[0] = doorObjectValue.substring(48,50);
            this.doorObjectUnknown2[0] = doorObjectValue.substring(50,52);
            this.doorObjectDoorType[0] = doorObjectValue.substring(52,54);
            this.doorObjectDoorLock[0] = doorObjectValue.substring(54,56);
            this.doorObjectUnknown3[0] = doorObjectValue.substring(56,58);
            this.doorObjectDoorLocked[0] = doorObjectValue.substring(58,60);
            this.doorObjectDoorKey[0] = doorObjectValue.substring(60,62);
            this.doorObjectUnknown4[0] = doorObjectValue.substring(62,64);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + doorObjectId[0];
        newScdDataByteString = newScdDataByteString + doorObjectUnknown1[0];
        newScdDataByteString = newScdDataByteString + doorObjectUnknown1[1];
        newScdDataByteString = newScdDataByteString + doorObjectUnknown1[2];
        newScdDataByteString = newScdDataByteString + doorObjectUnknown1[3];
        newScdDataByteString = newScdDataByteString + doorObjectDoorLocationX[0];
        newScdDataByteString = newScdDataByteString + doorObjectDoorLocationX[1];
        newScdDataByteString = newScdDataByteString + doorObjectDoorLocationY[0];
        newScdDataByteString = newScdDataByteString + doorObjectDoorLocationY[1];
        newScdDataByteString = newScdDataByteString + doorObjectDoorWidth[0];
        newScdDataByteString = newScdDataByteString + doorObjectDoorWidth[1];
        newScdDataByteString = newScdDataByteString + doorObjectDoorHeight[0];
        newScdDataByteString = newScdDataByteString + doorObjectDoorHeight[1];
        newScdDataByteString = newScdDataByteString + doorObjectPlayerPositionAfterEnteringX[0];
        newScdDataByteString = newScdDataByteString + doorObjectPlayerPositionAfterEnteringX[1];
        newScdDataByteString = newScdDataByteString + doorObjectPlayerPositionAfterEnteringY[0];
        newScdDataByteString = newScdDataByteString + doorObjectPlayerPositionAfterEnteringY[1];
        newScdDataByteString = newScdDataByteString + doorObjectPlayerPositionAfterEnteringZ[0];
        newScdDataByteString = newScdDataByteString + doorObjectPlayerPositionAfterEnteringZ[1];
        newScdDataByteString = newScdDataByteString + doorObjectPlayerPositionAfterEnteringNextDirection[0];
        newScdDataByteString = newScdDataByteString + doorObjectPlayerPositionAfterEnteringNextDirection[1];
        newScdDataByteString = newScdDataByteString + doorObjectStageAfterEntering[0];
        newScdDataByteString = newScdDataByteString + doorObjectRoomAfterEntering[0];
        newScdDataByteString = newScdDataByteString + doorObjectCameraAfterEntering[0];
        newScdDataByteString = newScdDataByteString + doorObjectUnknown2[0];
        newScdDataByteString = newScdDataByteString + doorObjectDoorType[0];
        newScdDataByteString = newScdDataByteString + doorObjectDoorLock[0];
        newScdDataByteString = newScdDataByteString + doorObjectUnknown3[0];
        newScdDataByteString = newScdDataByteString + doorObjectDoorLocked[0];
        newScdDataByteString = newScdDataByteString + doorObjectDoorKey[0];
        newScdDataByteString = newScdDataByteString + doorObjectUnknown4[0];

        return newScdDataByteString;
    }
}