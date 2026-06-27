package baubabi.reeditor.core.rdt.scd._67_op43_plc_flg;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class PlcFlg_plcFlg_43 {
    //4 Byte

    /*
        Player flag something? Fully unknown.
    */

    //1 byte
    private final String OPCODE = "43";

    //3 byte - unknown value
    private final String[] plcFlgUnknown = new String[3];

    private final int size = 4;

    private final String displayName = OPCODE + " - player character flag";

    public PlcFlg_plcFlg_43(String plcFlgValue) {
        if(plcFlgValue != null) {
            this.plcFlgUnknown[0] = plcFlgValue.substring(2,4);
            this.plcFlgUnknown[1] = plcFlgValue.substring(4,6);
            this.plcFlgUnknown[2] = plcFlgValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + plcFlgUnknown[0];
        newScdDataByteString = newScdDataByteString + plcFlgUnknown[1];
        newScdDataByteString = newScdDataByteString + plcFlgUnknown[2];

        return newScdDataByteString;
    }
}
