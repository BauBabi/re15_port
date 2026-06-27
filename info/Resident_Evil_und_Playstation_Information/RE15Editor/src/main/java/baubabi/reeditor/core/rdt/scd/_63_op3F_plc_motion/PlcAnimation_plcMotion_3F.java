package baubabi.reeditor.core.rdt.scd._63_op3F_plc_motion;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class PlcAnimation_plcMotion_3F {
    //4 Byte

    /*
        Player character animation maybe?
    */

    //1 byte
    private final String OPCODE = "3F"; /* 0x3f */

    //3 byte - unknown value
    private final String[] plcAnimationUnknown = new String[3];

    private final int size = 4;

    private final String displayName = "3F - player character animation";

    public PlcAnimation_plcMotion_3F(String plcAnimationValue) {
        if(plcAnimationValue != null) {
            this.plcAnimationUnknown[0] = plcAnimationValue.substring(2,4);
            this.plcAnimationUnknown[1] = plcAnimationValue.substring(4,6);
            this.plcAnimationUnknown[2] = plcAnimationValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + plcAnimationUnknown[0];
        newScdDataByteString = newScdDataByteString + plcAnimationUnknown[1];
        newScdDataByteString = newScdDataByteString + plcAnimationUnknown[2];

        return newScdDataByteString;
    }
}