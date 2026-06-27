package baubabi.reeditor.core.rdt.scd._116_op74_sce_fade_adjust;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceFadeAdjust_sceFadeAdjust_74 {
    //4 Byte

    /*
        Don't know the function of it
    */

    //1 byte
    private final String OPCODE = "74";

    //3 byte - the Sce_fade_adjust data
    private final String[] sceFadeAdjustData = new String[3];

    private final int size = 4;

    private final String displayName = OPCODE + " - sce fade adjust";

    public SceFadeAdjust_sceFadeAdjust_74(String sceFadeAdjustValue) {
        if(sceFadeAdjustValue != null) {
            this.sceFadeAdjustData[0] = sceFadeAdjustValue.substring(2, 4);
            this.sceFadeAdjustData[1] = sceFadeAdjustValue.substring(4, 6);
            this.sceFadeAdjustData[2] = sceFadeAdjustValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceFadeAdjustData[0];
        newScdDataByteString = newScdDataByteString + sceFadeAdjustData[1];
        newScdDataByteString = newScdDataByteString + sceFadeAdjustData[2];

        return newScdDataByteString;
    }
}
