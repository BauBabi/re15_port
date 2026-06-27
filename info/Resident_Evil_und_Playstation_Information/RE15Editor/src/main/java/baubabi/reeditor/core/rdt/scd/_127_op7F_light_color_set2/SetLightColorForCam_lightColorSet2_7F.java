package baubabi.reeditor.core.rdt.scd._127_op7F_light_color_set2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetLightColorForCam_lightColorSet2_7F {
    //6 Byte

    /*
        Set light color, a given camera.
    */

    //1 byte
    private final String OPCODE = "7F"; /* 0x7f */

    //1 byte - the camera to set light color for
    private final String[] setLightColorForCamCamera = new String[1]; /* Camera */

    //1 byte - the light for the camera
    private final String[] setLightColorForCamLight = new String[1]; /* Light */

    //1 byte - the red value of color
    private final String[] setLightColorForCamRed = new String[1]; /* R,G,B component of color */

    //1 byte - the green value of color
    private final String[] setLightColorForCamGreen = new String[1]; /* R,G,B component of color */

    //1 byte - the blue value of color
    private final String[] setLightColorForCamBlue = new String[1]; /* R,G,B component of color */

    private final int size = 6;

    private final String displayName = OPCODE + " - set light color for cam";

    public SetLightColorForCam_lightColorSet2_7F(String setLightColorForCamValue) {
        if(setLightColorForCamValue != null) {
            this.setLightColorForCamCamera[0] = setLightColorForCamValue.substring(2, 4);
            this.setLightColorForCamLight[0] = setLightColorForCamValue.substring(4, 6);
            this.setLightColorForCamRed[0] = setLightColorForCamValue.substring(6, 8);
            this.setLightColorForCamGreen[0] = setLightColorForCamValue.substring(8, 10);
            this.setLightColorForCamBlue[0] = setLightColorForCamValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setLightColorForCamCamera[0];
        newScdDataByteString = newScdDataByteString + setLightColorForCamLight[0];
        newScdDataByteString = newScdDataByteString + setLightColorForCamRed[0];
        newScdDataByteString = newScdDataByteString + setLightColorForCamGreen[0];
        newScdDataByteString = newScdDataByteString + setLightColorForCamBlue[0];

        return newScdDataByteString;
    }
}
