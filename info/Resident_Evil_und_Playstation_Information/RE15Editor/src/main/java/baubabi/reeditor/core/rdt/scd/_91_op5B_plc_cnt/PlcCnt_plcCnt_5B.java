package baubabi.reeditor.core.rdt.scd._91_op5B_plc_cnt;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class PlcCnt_plcCnt_5B {
    //2 Byte

    /*
        Don't know, something with player
    */

    //1 byte
    private final String OPCODE = "5B"; /* 0x5b */

    //1 byte - unknown value
    private final String[] plcCntUnknown = new String[1];

    private final int size = 2;

    private final String displayName = OPCODE + " - plc cnt";

    public PlcCnt_plcCnt_5B(String plcCntValue) {
        if(plcCntValue != null) {
            this.plcCntUnknown[0] = plcCntValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + plcCntUnknown[0];

        return newScdDataByteString;
    }
}
