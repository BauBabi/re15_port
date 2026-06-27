package baubabi.reeditor.core.rdt.scd._126_op7E_light_kido_set2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ChangeLightBrightnessForCam_lightKidoSet2_7E {
    //6 Byte

    /*
        Change light brightness, for a given camera.
    */

    //1 byte
    private final String OPCODE = "7E"; /* 0x7e */

    //1 byte - a dummy Value
    private final String[] changeLightBrightnessForCamDummy = new String[1];

    //1 byte - the camera to set brightness for
    private final String[] changeLightBrightnessForCamCamera = new String[1]; /* Camera */

    //1 byte - the light to set brightness
    private final String[] changeLightBrightnessForCamLight = new String[1]; /* Light */

    //2 byte - the brightness value
    private final String[] changeLightBrightnessForCamBrightness = new String[2]; /* New brightness */

    private final int size = 6;

    private final String displayName = OPCODE + " - change light brightness for cam";

    public ChangeLightBrightnessForCam_lightKidoSet2_7E(String changeLightBrightnessForCamValue) {
        if(changeLightBrightnessForCamValue != null) {
            this.changeLightBrightnessForCamDummy[0] = changeLightBrightnessForCamValue.substring(2, 4);
            this.changeLightBrightnessForCamCamera[0] = changeLightBrightnessForCamValue.substring(4, 6);
            this.changeLightBrightnessForCamLight[0] = changeLightBrightnessForCamValue.substring(6, 8);
            this.changeLightBrightnessForCamBrightness[0] = changeLightBrightnessForCamValue.substring(8, 10);
            this.changeLightBrightnessForCamBrightness[1] = changeLightBrightnessForCamValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + changeLightBrightnessForCamDummy[0];
        newScdDataByteString = newScdDataByteString + changeLightBrightnessForCamCamera[0];
        newScdDataByteString = newScdDataByteString + changeLightBrightnessForCamLight[0];
        newScdDataByteString = newScdDataByteString + changeLightBrightnessForCamBrightness[0];
        newScdDataByteString = newScdDataByteString + changeLightBrightnessForCamBrightness[1];

        return newScdDataByteString;
    }
}
