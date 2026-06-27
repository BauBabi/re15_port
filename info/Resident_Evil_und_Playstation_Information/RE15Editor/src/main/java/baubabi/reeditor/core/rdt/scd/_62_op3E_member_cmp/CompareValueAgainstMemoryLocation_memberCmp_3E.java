package baubabi.reeditor.core.rdt.scd._62_op3E_member_cmp;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class CompareValueAgainstMemoryLocation_memberCmp_3E {
    //6 Byte

    /*
        Compare immediate value against a memory location.
    */

    //1 byte
    private final String OPCODE = "3E"; /* 0x3e */

    //1 byte - unknown value 1
    private final String[] compareValueAgainstMemoryLocationUnknown1 = new String[1];

    //1 byte - unknown value 2
    private final String[] compareValueAgainstMemoryLocationUnknown2 = new String[1];

    //1 byte - compare operation
    /* Compare function */
    /* 0: EQ */
    /* 1: GT */
    /* 2: GE */
    /* 3: LT */
    /* 4: LE */
    /* 5: NE */
    /* 6: ? */
    private final String[] compareValueAgainstMemoryLocationCompareOperation = new String[1];

    //2 byte - the value to compare against
    private final String[] compareValueAgainstMemoryLocationCompareValue = new String[2];

    private final int size = 6;

    private final String displayName = "3E - compare value against memory location";

    public CompareValueAgainstMemoryLocation_memberCmp_3E(String compareValueAgainstMemoryLocationValue) {
        if(compareValueAgainstMemoryLocationValue != null) {
            this.compareValueAgainstMemoryLocationUnknown1[0] = compareValueAgainstMemoryLocationValue.substring(2,4);
            this.compareValueAgainstMemoryLocationUnknown2[0] = compareValueAgainstMemoryLocationValue.substring(4,6);
            this.compareValueAgainstMemoryLocationCompareOperation[0] = compareValueAgainstMemoryLocationValue.substring(6,8);
            this.compareValueAgainstMemoryLocationCompareValue[0] = compareValueAgainstMemoryLocationValue.substring(8,10);
            this.compareValueAgainstMemoryLocationCompareValue[1] = compareValueAgainstMemoryLocationValue.substring(10,12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + compareValueAgainstMemoryLocationUnknown1[0];
        newScdDataByteString = newScdDataByteString + compareValueAgainstMemoryLocationUnknown2[0];
        newScdDataByteString = newScdDataByteString + compareValueAgainstMemoryLocationCompareOperation[0];
        newScdDataByteString = newScdDataByteString + compareValueAgainstMemoryLocationCompareValue[0];
        newScdDataByteString = newScdDataByteString + compareValueAgainstMemoryLocationCompareValue[1];

        return newScdDataByteString;
    }
}
