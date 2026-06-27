package baubabi.reeditor.core.rdt.scd._106_op6A_light_pos_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetLightPositionComponent_lightPosSet_6A {
    //6 Byte

    /*
        Change one of the light position components, for current camera.
    */

    //1 byte
    private final String OPCODE = "6A"; /* 0x6a */

    //1 byte - some dummy value
    private final String[] setLightPositionComponentDummy = new String[1];

    //1 byte - the number of light to change
    private final String[] setLightPositionComponentLight = new String[1]; /* Number of light to change */

    //1 byte - the parameter
    private final String[] setLightPositionComponentParam = new String[1]; /* 11,12,13 for x,y,z */

    //2 byte - the new value for parameter
    private final String[] setLightPositionComponentValue = new String[2]; /* New value for parameter */

    private final int size = 6;

    private final String displayName = OPCODE + " - change light position for cam";

    public SetLightPositionComponent_lightPosSet_6A(String setLightPositionComponentValue) {
        if(setLightPositionComponentValue != null) {
            this.setLightPositionComponentDummy[0] = setLightPositionComponentValue.substring(2, 4);
            this.setLightPositionComponentLight[0] = setLightPositionComponentValue.substring(4, 6);
            this.setLightPositionComponentParam[0] = setLightPositionComponentValue.substring(6, 8);
            this.setLightPositionComponentValue[0] = setLightPositionComponentValue.substring(8, 10);
            this.setLightPositionComponentValue[1] = setLightPositionComponentValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setLightPositionComponentDummy[0];
        newScdDataByteString = newScdDataByteString + setLightPositionComponentLight[0];
        newScdDataByteString = newScdDataByteString + setLightPositionComponentParam[0];
        newScdDataByteString = newScdDataByteString + setLightPositionComponentValue[0];
        newScdDataByteString = newScdDataByteString + setLightPositionComponentValue[1];

        return newScdDataByteString;
    }
}
