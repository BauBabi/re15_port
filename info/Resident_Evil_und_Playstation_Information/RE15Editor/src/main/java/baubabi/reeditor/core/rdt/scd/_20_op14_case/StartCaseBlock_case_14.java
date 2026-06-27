package baubabi.reeditor.core.rdt.scd._20_op14_case;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class StartCaseBlock_case_14 {
    //6 Byte

    /*
        Start of CASE block.

              Execute a block of instructions if 'varw' variable of Switch instruction equals 'value'.
    */

    //1 byte
    private final String OPCODE = "14";

    //1 byte - case dummy
    private final String[] startCaseBlockDummy = new String[1];

    //2 byte - case block length
    private final String[] startCaseBlockBlockLength = new String[2];

    //2 byte - the value
    private final String[] startCaseBlockValue = new String[2];

    private final int size = 6;

    private final String displayName = "14 - start case block";

    public StartCaseBlock_case_14(String startCaseBlockValue) {
        if(startCaseBlockValue != null) {
            this.startCaseBlockDummy[0] = startCaseBlockValue.substring(2,4);
            this.startCaseBlockBlockLength[0] = startCaseBlockValue.substring(4,6);
            this.startCaseBlockBlockLength[1] = startCaseBlockValue.substring(6,8);
            this.startCaseBlockValue[0] = startCaseBlockValue.substring(8,10);
            this.startCaseBlockValue[1] = startCaseBlockValue.substring(10,12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + startCaseBlockDummy[0];
        newScdDataByteString = newScdDataByteString + startCaseBlockBlockLength[0];
        newScdDataByteString = newScdDataByteString + startCaseBlockBlockLength[1];
        newScdDataByteString = newScdDataByteString + startCaseBlockValue[0];
        newScdDataByteString = newScdDataByteString + startCaseBlockValue[1];

        return newScdDataByteString;
    }
}
