package baubabi.reeditor.core.rdt.scd._85_op55_member_calc;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class MemberCalc_memberCalc_55 {
    //6 Byte

    /*
        Something with calculation
    */

    //1 byte
    private final String OPCODE = "55"; /* 0x55 */

    //1 byte - a dummy
    private final String[] memberCalcDummy = new String[1];

    //4 byte - unknown value - Array of 2 with 2 byte each
    private final String[] memberCalcUnknown = new String[4];

    private final int size = 6;

    private final String displayName = OPCODE + " - member calc";

    public MemberCalc_memberCalc_55(String memberCalcValue) {
        if(memberCalcValue != null) {
            this.memberCalcDummy[0] = memberCalcValue.substring(2, 4);
            this.memberCalcUnknown[0] = memberCalcValue.substring(4, 6);
            this.memberCalcUnknown[1] = memberCalcValue.substring(6, 8);
            this.memberCalcUnknown[2] = memberCalcValue.substring(8, 10);
            this.memberCalcUnknown[3] = memberCalcValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + memberCalcDummy[0];
        newScdDataByteString = newScdDataByteString + memberCalcUnknown[0];
        newScdDataByteString = newScdDataByteString + memberCalcUnknown[1];
        newScdDataByteString = newScdDataByteString + memberCalcUnknown[2];
        newScdDataByteString = newScdDataByteString + memberCalcUnknown[3];

        return newScdDataByteString;
    }
}
