package baubabi.reeditor.core.rdt.scd._55_op37_sca_id_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ChangeCamera_scaIdSet_37 {
    //4 Byte

    /*
        Change camera
    */

    //1 byte
    private final String OPCODE = "37";

    //3 byte - The data to change camera
    private final String[] changeCameraData = new String[3];

    private final int size = 4;

    private final String displayName = "37 - change camera";

    public ChangeCamera_scaIdSet_37(String changeCameraValue) {
        if(changeCameraValue != null) {
            this.changeCameraData[0] = changeCameraValue.substring(2, 4);
            this.changeCameraData[1] = changeCameraValue.substring(4, 6);
            this.changeCameraData[2] = changeCameraValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + changeCameraData[0];
        newScdDataByteString = newScdDataByteString + changeCameraData[1];
        newScdDataByteString = newScdDataByteString + changeCameraData[2];

        return newScdDataByteString;
    }
}
