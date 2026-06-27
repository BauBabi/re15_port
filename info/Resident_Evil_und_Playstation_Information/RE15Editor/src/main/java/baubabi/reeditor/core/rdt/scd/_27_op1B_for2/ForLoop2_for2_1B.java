package baubabi.reeditor.core.rdt.scd._27_op1B_for2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ForLoop2_for2_1B {
    //6 Byte

    /*
 	    Initialize second FOR loop somehow.
    */

    //1 byte
    private final String OPCODE = "1B";

    //1 byte - an unknown value 1
    private final String[] forLoop2Unknown1= new String[1];

    //2 byte - an unknown value 2
    private final String[] forLoop2Unknown2 = new String[2];

    //2 byte - an unknown value 3
    private final String[] forLoop2Unknown3 = new String[2];

    private final int size = 6;

    private final String displayName = "1B - initialize second for loop";

    public ForLoop2_for2_1B(String forLoop2Value) {
        if(forLoop2Value != null) {
            this.forLoop2Unknown1[0] = forLoop2Value.substring(2,4);
            this.forLoop2Unknown2[0] = forLoop2Value.substring(4,6);
            this.forLoop2Unknown2[1] = forLoop2Value.substring(6,8);
            this.forLoop2Unknown3[0] = forLoop2Value.substring(8,10);
            this.forLoop2Unknown3[1] = forLoop2Value.substring(10,12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + forLoop2Unknown1[0];
        newScdDataByteString = newScdDataByteString + forLoop2Unknown2[0];
        newScdDataByteString = newScdDataByteString + forLoop2Unknown2[1];
        newScdDataByteString = newScdDataByteString + forLoop2Unknown3[0];
        newScdDataByteString = newScdDataByteString + forLoop2Unknown3[1];

        return newScdDataByteString;
    }
}
