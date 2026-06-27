package baubabi.reeditor.core.rdt.scd._56_op38_flr_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class FloorSet_flrSet_38 {
    //3 Byte

    /*
        Sets the floor data?
    */

    //1 byte
    private final String OPCODE = "38"; /* 0x38 */

    //1 byte - index of array at offset 11 of file
    private final String[] floorSetId = new String[1]; /* Index in array of object at offset 11 of file */

    //1 byte - Flag to clear/set bit 15 of last word of object at offset 11
    private final String[] floorSeFlag = new String[1]; /* Flag to clear or set bit 15 of last word of object at offset 11 */

    private final int size = 3;

    private final String displayName = "38 - set floor data";

    public FloorSet_flrSet_38(String floorSetValue) {
        if(floorSetValue != null) {
            this.floorSetId[0] = floorSetValue.substring(2, 4);
            this.floorSeFlag[1] = floorSetValue.substring(4, 6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + floorSetId[0];
        newScdDataByteString = newScdDataByteString + floorSeFlag[0];

        return newScdDataByteString;
    }
}
