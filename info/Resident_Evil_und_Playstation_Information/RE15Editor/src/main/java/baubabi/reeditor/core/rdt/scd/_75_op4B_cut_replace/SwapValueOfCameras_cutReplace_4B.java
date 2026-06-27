package baubabi.reeditor.core.rdt.scd._75_op4B_cut_replace;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SwapValueOfCameras_cutReplace_4B {
    //3 Byte

    /*
        Browse the list of camera switches (offset 8 of file), swapping all values of camera 'from' and 'to' that matches cam[0] and cam[1].
    */

    //1 byte
    private final String OPCODE = "4B"; /* 0x4b */

    //2 byte - The cameras to switch - from to
    private final String[] swapValueOfCamerasCameras = new String[2];

    private final int size = 3;

    private final String displayName = OPCODE + " - swap camera switches";

    public SwapValueOfCameras_cutReplace_4B(String swapValueOfCamerasValue) {
        if(swapValueOfCamerasValue != null) {
            this.swapValueOfCamerasCameras[0] = swapValueOfCamerasValue.substring(2, 4);
            this.swapValueOfCamerasCameras[1] = swapValueOfCamerasValue.substring(4, 6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + swapValueOfCamerasCameras[0];
        newScdDataByteString = newScdDataByteString + swapValueOfCamerasCameras[1];

        return newScdDataByteString;
    }
}
