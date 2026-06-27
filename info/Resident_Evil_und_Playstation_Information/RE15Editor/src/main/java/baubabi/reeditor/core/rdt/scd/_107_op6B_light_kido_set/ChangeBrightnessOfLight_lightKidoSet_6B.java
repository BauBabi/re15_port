package baubabi.reeditor.core.rdt.scd._107_op6B_light_kido_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ChangeBrightnessOfLight_lightKidoSet_6B {
    //4 Byte

    /*
        Change brightness of one light, for current camera.
    */

    //1 byte
    private final String OPCODE = "6B"; /* 0x6b */

    //1 byte - the number of light
    private final String[] changeBrightnessOfLightLightNumber = new String[1]; /* Number of light */

    //2 byte - value of brightness
    private final String[] changeBrightnessOfLightBrightnessValue = new String[2]; /* New value for brightness */

    private final int size = 4;

    private final String displayName = OPCODE + " - change brightness of light for cam";

    public ChangeBrightnessOfLight_lightKidoSet_6B(String changeBrightnessOfLightValue) {
        if(changeBrightnessOfLightValue != null) {
            this.changeBrightnessOfLightLightNumber[0] = changeBrightnessOfLightValue.substring(2, 4);
            this.changeBrightnessOfLightBrightnessValue[0] = changeBrightnessOfLightValue.substring(4, 6);
            this.changeBrightnessOfLightBrightnessValue[1] = changeBrightnessOfLightValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + changeBrightnessOfLightLightNumber[0];
        newScdDataByteString = newScdDataByteString + changeBrightnessOfLightBrightnessValue[0];
        newScdDataByteString = newScdDataByteString + changeBrightnessOfLightBrightnessValue[1];

        return newScdDataByteString;
    }
}
