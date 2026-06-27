package baubabi.reeditor.core.rdt.scd._86_op56_member_calc2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class MemberCalc2_memberCalc2_56 {
    //4 Byte

    /*
        Something with calculation
    */

    //1 byte
    private final String OPCODE = "56"; /* 0x56 */

    //3 byte - unknown value - Array of 3 with 1 byte each
    private final String[] memberCalc2Unknown = new String[3];

    private final int size = 4;

    private final String displayName = OPCODE + " - member calc 2";

    public MemberCalc2_memberCalc2_56(String memberCalc2Value) {
        if(memberCalc2Value != null) {
            this.memberCalc2Unknown[0] = memberCalc2Value.substring(2, 4);
            this.memberCalc2Unknown[1] = memberCalc2Value.substring(4, 6);
            this.memberCalc2Unknown[2] = memberCalc2Value.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + memberCalc2Unknown[0];
        newScdDataByteString = newScdDataByteString + memberCalc2Unknown[1];
        newScdDataByteString = newScdDataByteString + memberCalc2Unknown[2];

        return newScdDataByteString;
    }
}
