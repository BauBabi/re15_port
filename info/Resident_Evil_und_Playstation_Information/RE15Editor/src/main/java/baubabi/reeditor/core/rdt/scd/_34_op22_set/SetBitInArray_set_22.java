package baubabi.reeditor.core.rdt.scd._34_op22_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetBitInArray_set_22 {
    //4 Byte

    /*
        Change bit in array.
    */

    //1 byte
    private final String OPCODE = "22"; /* 0x22 */

    //1 byte - Index of Array of Bits to use
    private final String[] setBitInArrayBitArray = new String[1]; /* Index of array of bits to use (see end of section) */

    //1 byte - Bit Number to Check
    private final String[] setBitInArrayNumber = new String[1]; /* Bit number to check */

    //1 byte - Operation on bit
    /* Operation on bit */
    /* 0x00: CLEAR */
    /* 0x01: SET */
    /* 0x02-0x06: INVALID */
    /* 0x07: CHANGE */
    private final String[] setBitInArrayOpChg = new String[1];

    private final int size = 4;

    private final String displayName = "22 - set bit in array";

    public SetBitInArray_set_22(String setBitInArrayValue) {
        if(setBitInArrayValue != null) {
            this.setBitInArrayBitArray[0] = setBitInArrayValue.substring(2,4);
            this.setBitInArrayNumber[0] = setBitInArrayValue.substring(4,6);
            this.setBitInArrayOpChg[0] = setBitInArrayValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setBitInArrayBitArray[0];
        newScdDataByteString = newScdDataByteString + setBitInArrayNumber[0];
        newScdDataByteString = newScdDataByteString + setBitInArrayOpChg[0];

        return newScdDataByteString;
    }
}
