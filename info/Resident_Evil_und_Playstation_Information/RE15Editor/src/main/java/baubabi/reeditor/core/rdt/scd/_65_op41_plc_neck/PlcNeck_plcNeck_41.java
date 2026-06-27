package baubabi.reeditor.core.rdt.scd._65_op41_plc_neck;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class PlcNeck_plcNeck_41 {
    //10 Byte

    /*
        No Clue player neck?
    */

    //1 byte
    private final String OPCODE = "41"; /* 0x41 */

    //1 byte - unknown value 1
    private final String[] plcNeckUnknown1 = new String[1];

    //8 byte - unknown value 2 - array of 4 with 2 byte each
    private final String[] plcNeckUnknown2 = new String[8];

    private final int size = 10;

    private final String displayName = "41 - player character neck";

    public PlcNeck_plcNeck_41(String plcNeckValue) {
        if(plcNeckValue != null) {
            this.plcNeckUnknown1[0] = plcNeckValue.substring(2,4);
            this.plcNeckUnknown2[0] = plcNeckValue.substring(4,6);
            this.plcNeckUnknown2[1] = plcNeckValue.substring(6,8);
            this.plcNeckUnknown2[2] = plcNeckValue.substring(8,10);
            this.plcNeckUnknown2[3] = plcNeckValue.substring(10,12);
            this.plcNeckUnknown2[4] = plcNeckValue.substring(12,14);
            this.plcNeckUnknown2[5] = plcNeckValue.substring(14,16);
            this.plcNeckUnknown2[6] = plcNeckValue.substring(16,18);
            this.plcNeckUnknown2[7] = plcNeckValue.substring(18,20);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + plcNeckUnknown1[0];
        newScdDataByteString = newScdDataByteString + plcNeckUnknown2[0];
        newScdDataByteString = newScdDataByteString + plcNeckUnknown2[1];
        newScdDataByteString = newScdDataByteString + plcNeckUnknown2[2];
        newScdDataByteString = newScdDataByteString + plcNeckUnknown2[3];
        newScdDataByteString = newScdDataByteString + plcNeckUnknown2[4];
        newScdDataByteString = newScdDataByteString + plcNeckUnknown2[5];
        newScdDataByteString = newScdDataByteString + plcNeckUnknown2[6];
        newScdDataByteString = newScdDataByteString + plcNeckUnknown2[7];

        return newScdDataByteString;
    }
}
