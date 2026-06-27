package baubabi.reeditor.core.rdt.scd._53_op35_member_set2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class MemberSet2_memberSet2_35 {
    //3 Byte

    /*
        Copy content varw variable value to a given value for current entity.
    */

    //1 byte
    private final String OPCODE = "35"; /* 0x35 */

    //1 byte - The id of value to change
    /* Value to change for current entity */
    /* 0x0b: X pos */
    /* 0x0c: Y pos */
    /* 0x0d: Z pos */
    /* 0x0e: X angle */
    /* 0x0f: Y angle */
    /* 0x10: Z angle */
    private final String[] memberSet2Id = new String[1];

    //1 byte - The Variable to use
    private final String[] memberSet2Varw = new String[1]; /* Variable to use */

    private final int size = 3;

    private final String displayName = "35 - member set2";


    public MemberSet2_memberSet2_35(String memberSet2Value) {
        if(memberSet2Value != null) {
            this.memberSet2Id[0] = memberSet2Value.substring(2,4);
            this.memberSet2Varw[0] = memberSet2Value.substring(4,6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + memberSet2Id[0];
        newScdDataByteString = newScdDataByteString + memberSet2Varw[0];

        return newScdDataByteString;
    }
}
