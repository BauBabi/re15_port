package baubabi.reeditor.core.rdt.scd._125_op7D_light_pos_set2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetLightColorForCam_lightPosSet2_7D {
    //6 Byte

    /*
        Change one of the light position components, for a given camera.
    */

    //1 byte
    private final String OPCODE = "7D"; /* 0x7d */

    //1 byte - the camera for changing light position
    private final String[] setLightColorForCamCamera = new String[1]; /* Camera */

    //1 byte - the light of the camera
    private final String[] setLightColorForCamLight = new String[1]; /* Light */

    //1 byte - the parameter for the value
    private final String[] setLightColorForCamParam = new String[1]; /* 11,12,13 for x,y,z */

    //2 byte - the value
    private final String[] setLightColorForCamValue = new String[2]; /* New value for parameter */

    private final int size = 6;

    private final String displayName = OPCODE + " - change light position for cam";

    public SetLightColorForCam_lightPosSet2_7D(String setLightColorForCamValue) {
        if(setLightColorForCamValue != null) {
            this.setLightColorForCamCamera[0] = setLightColorForCamValue.substring(2, 4);
            this.setLightColorForCamLight[0] = setLightColorForCamValue.substring(4, 6);
            this.setLightColorForCamParam[0] = setLightColorForCamValue.substring(6, 8);
            this.setLightColorForCamValue[0] = setLightColorForCamValue.substring(8, 10);
            this.setLightColorForCamValue[1] = setLightColorForCamValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setLightColorForCamCamera[0];
        newScdDataByteString = newScdDataByteString + setLightColorForCamLight[0];
        newScdDataByteString = newScdDataByteString + setLightColorForCamParam[0];
        newScdDataByteString = newScdDataByteString + setLightColorForCamValue[0];
        newScdDataByteString = newScdDataByteString + setLightColorForCamValue[1];

        return newScdDataByteString;
    }
}
