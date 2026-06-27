package baubabi.reeditor.core.rdt.scd._35_op23_compare;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class CompareValueAgainstArrayValue_compare_23 {
    //6 Byte

    /*
        Compare a value against one in an internal array.
    */

    //1 byte
    private final String OPCODE = "23"; /* 0x23 */

    //1 byte - Compare dummy
    private final String[] compareValueAgainstArrayValueDummy = new String[1];

    //1 byte - The Index of the array
    private final String[] compareValueAgainstArrayValueArrayIndex = new String[1]; /* Index in array */

    //1 byte - The compare function
    /* Compare function */
    /* 0: EQ */
    /* 1: GT */
    /* 2: GE */
    /* 3: LT */
    /* 4: LE */
    /* 5: NE */
    /* 6: ? */
    private final String[] compareValueAgainstArrayValueCompare = new String[1];

    //2 byte - The value to compare against
    private final String[] compareValueAgainstArrayValueValueToCompareAgainst = new String[2];

    private final int size = 6;

    private final String displayName = "23 - compare value against array value";

    public CompareValueAgainstArrayValue_compare_23(String compareValueAgainstArrayValueValue) {
        if(compareValueAgainstArrayValueValue != null) {
            this.compareValueAgainstArrayValueDummy[0] = compareValueAgainstArrayValueValue.substring(2,4);
            this.compareValueAgainstArrayValueArrayIndex[0] = compareValueAgainstArrayValueValue.substring(4,6);
            this.compareValueAgainstArrayValueCompare[0] = compareValueAgainstArrayValueValue.substring(6,8);
            this.compareValueAgainstArrayValueValueToCompareAgainst[0] = compareValueAgainstArrayValueValue.substring(8,10);
            this.compareValueAgainstArrayValueValueToCompareAgainst[1] = compareValueAgainstArrayValueValue.substring(10,12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + compareValueAgainstArrayValueDummy[0];
        newScdDataByteString = newScdDataByteString + compareValueAgainstArrayValueArrayIndex[0];
        newScdDataByteString = newScdDataByteString + compareValueAgainstArrayValueCompare[0];
        newScdDataByteString = newScdDataByteString + compareValueAgainstArrayValueValueToCompareAgainst[0];
        newScdDataByteString = newScdDataByteString + compareValueAgainstArrayValueValueToCompareAgainst[1];

        return newScdDataByteString;
    }
}
