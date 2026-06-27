package baubabi.reeditor.core.rdt.scd._124_op7C_light_color_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetCamLightColor_lightColorSet_7C {
    //6 Byte

    /*
        Set color of one light, for current camera
    */

    //1 byte
    private final String OPCODE = "7C"; /* 0x7c */

    //1 byte - set number of light color
    private final String[] setCamLightColorLightColor = new String[1]; /* Number of color to set for current camera */

    //1 byte - value of color
    private final String[] setCamLightColorRed = new String[1]; /* R,G,B component of color */

    //1 byte - value of color
    private final String[] setCamLightColorGreen = new String[1]; /* R,G,B component of color */

    //1 byte - value of color
    private final String[] setCamLightColorBlue = new String[1]; /* R,G,B component of color */

    //1 byte - dummy value
    private final String[] setCamLightColorDummy = new String[1];

    private final int size = 6;

    private final String displayName = OPCODE + " - set cam light color";

    public SetCamLightColor_lightColorSet_7C(String setCamLightColorValue) {
        if(setCamLightColorValue != null) {
            this.setCamLightColorLightColor[0] = setCamLightColorValue.substring(2, 4);
            this.setCamLightColorRed[0] = setCamLightColorValue.substring(4, 6);
            this.setCamLightColorGreen[0] = setCamLightColorValue.substring(6, 8);
            this.setCamLightColorBlue[0] = setCamLightColorValue.substring(8, 10);
            this.setCamLightColorDummy[0] = setCamLightColorValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setCamLightColorLightColor[0];
        newScdDataByteString = newScdDataByteString + setCamLightColorRed[0];
        newScdDataByteString = newScdDataByteString + setCamLightColorGreen[0];
        newScdDataByteString = newScdDataByteString + setCamLightColorBlue[0];
        newScdDataByteString = newScdDataByteString + setCamLightColorDummy[0];

        return newScdDataByteString;
    }
}
