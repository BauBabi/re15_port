package baubabi.reeditor.core.rdt.scd._33_op21_ck;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class TestBitInArray_ck_21 {
    //4 Byte

    /*
        Test bit in array.
    */

    //1 byte
    private final String OPCODE = "21"; /* 0x21 */

    //1 byte - Index of Array of bits
    private final String[] testBitInArrayBitArray = new String[1]; /* Index of array of bits to use (see end of section) */

    //1 Byte - Bit Number to check
    private final String[] testBitInArrayNumber = new String[1]; /* Bit number to check */

    //1 Byte - Value to compare
    private final String[] testBitInArrayValue = new String[1]; /* Value to compare (0 or 1) */

    private final int size = 4;

    private final String displayName = "21 - test bit in array";

    public TestBitInArray_ck_21(String testBitInArrayValue) {
        if(testBitInArrayValue != null) {
            this.testBitInArrayBitArray[0] = testBitInArrayValue.substring(2,4);
            this.testBitInArrayNumber[0] = testBitInArrayValue.substring(4,6);
            this.testBitInArrayValue[0] = testBitInArrayValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + testBitInArrayBitArray[0];
        newScdDataByteString = newScdDataByteString + testBitInArrayNumber[0];
        newScdDataByteString = newScdDataByteString + testBitInArrayValue[0];

        return newScdDataByteString;
    }
}
