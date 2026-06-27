package baubabi.reeditor.core.rdt.scd._44_op2C_aot_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class DefineNonPickableObject_aotSet_2C {
    //20 Byte

    /*
        Non pickable object (Obstacle, message when examining something, dead corpse, fire)
        The non-pickable object is added to the room objects list.
    */

    //1 byte
    private final String OPCODE = "2C"; /* 0x2c */

    //1 byte - index of item in array of the room object list
    private final String[] defineNonPickableObjectIndex = new String[1]; /* Index of item in array of room objects list */

    //1 byte - Type of the non pickable object
    private final String[] defineNonPickableObjectType = new String[1];

    //3 byte - unknown value 1 - Its an Array of [3] with 1 byte each
    private final String[] defineNonPickableObjectUnknown1 = new String[3];

    //2 byte - X Position
    private final String[] defineNonPickableObjectX = new String[2];

    //2 byte - Y Position
    private final String[] defineNonPickableObjectY = new String[2];

    //2 byte - Width
    private final String[] defineNonPickableObjectWidth = new String[2];

    //2 byte - Height
    private final String[] defineNonPickableObjectHeight = new String[2];

    //6 byte - unknown value 2 - Its an Array of [3] with 2 byte each
    private final String[] defineNonPickableObjectUnknown2 = new String[6];

    //2 byte - Optional - it only get those 2 bytes, if defineNonPickableObjectUnknown1 starts with an B1
    private final String[] defineNonPickableObjectUnknownDataAdditional = new String[2];

    private int size = 20;

    private final String displayName = "2C - non pickable object";

    public DefineNonPickableObject_aotSet_2C(String defineNonPickableObjectValue) {
        if(defineNonPickableObjectValue != null) {
            initializeDefaultData(defineNonPickableObjectValue);
        }
    }
    public DefineNonPickableObject_aotSet_2C(String defineNonPickableObjectValue, boolean isBigFormat) {
        if(defineNonPickableObjectValue != null) {
            initializeDefaultData(defineNonPickableObjectValue);
            this.defineNonPickableObjectUnknownDataAdditional[0] = defineNonPickableObjectValue.substring(40,42);
            this.defineNonPickableObjectUnknownDataAdditional[1] = defineNonPickableObjectValue.substring(42,44);
            setDefineNotPickableObjectSize(22);
        }
    }

    public void initializeDefaultData(String defineNonPickableObjectValue) {
        this.defineNonPickableObjectIndex[0] = defineNonPickableObjectValue.substring(2,4);
        this.defineNonPickableObjectType[0] = defineNonPickableObjectValue.substring(4,6);
        this.defineNonPickableObjectUnknown1[0] = defineNonPickableObjectValue.substring(6,8);
        this.defineNonPickableObjectUnknown1[1] = defineNonPickableObjectValue.substring(8,10);
        this.defineNonPickableObjectUnknown1[2] = defineNonPickableObjectValue.substring(10,12);
        this.defineNonPickableObjectX[0] = defineNonPickableObjectValue.substring(12,14);
        this.defineNonPickableObjectX[1] = defineNonPickableObjectValue.substring(14,16);
        this.defineNonPickableObjectY[0] = defineNonPickableObjectValue.substring(16,18);
        this.defineNonPickableObjectY[1] = defineNonPickableObjectValue.substring(18,20);
        this.defineNonPickableObjectWidth[0] = defineNonPickableObjectValue.substring(20,22);
        this.defineNonPickableObjectWidth[1] = defineNonPickableObjectValue.substring(22,24);
        this.defineNonPickableObjectHeight[0] = defineNonPickableObjectValue.substring(24,26);
        this.defineNonPickableObjectHeight[1] = defineNonPickableObjectValue.substring(26,28);
        this.defineNonPickableObjectUnknown2[0] = defineNonPickableObjectValue.substring(28,30);
        this.defineNonPickableObjectUnknown2[1] = defineNonPickableObjectValue.substring(30,32);
        this.defineNonPickableObjectUnknown2[2] = defineNonPickableObjectValue.substring(32,34);
        this.defineNonPickableObjectUnknown2[3] = defineNonPickableObjectValue.substring(34,36);
        this.defineNonPickableObjectUnknown2[4] = defineNonPickableObjectValue.substring(36,38);
        this.defineNonPickableObjectUnknown2[5] = defineNonPickableObjectValue.substring(38,40);
    }


    //If the first value of defineNonPickableObjectUnknown1 starts with an B1, defineNonPickableObjectAotSetSize is 22 byte instead of 20.
    public void setDefineNotPickableObjectSize(int newDefineNonPickableObjectAotSetSize) {
        this.size = newDefineNonPickableObjectAotSetSize;
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectIndex[0];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectType[0];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknown1[0];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknown1[1];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknown1[2];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectX[0];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectX[1];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectY[0];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectY[1];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectWidth[0];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectWidth[1];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectHeight[0];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectHeight[1];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknown2[0];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknown2[1];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknown2[2];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknown2[3];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknown2[4];
        newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknown2[5];
        if(defineNonPickableObjectUnknownDataAdditional[0] != null) {
            newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknownDataAdditional[0];
            newScdDataByteString = newScdDataByteString + defineNonPickableObjectUnknownDataAdditional[1];
        }

        return newScdDataByteString;
    }
}
