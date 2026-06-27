package baubabi.reeditor.core.rdt.scd._88_op58_plc_rot;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class PlayerRotation_PlcRot_58 {
    //4 Byte

    /*
        Don't know - player rotation
    */

    //1 byte
    private final String OPCODE = "58"; /* 0x58 */

    //1 byte - unknown value 1
    private final String[] playerRotationUnknown1 = new String[1];

    //2 byte - unknown value 2
    private final String[] playerRotationUnknown2 = new String[2];

    private final int size = 4;

    private final String displayName = OPCODE + " - plc rot";

    public PlayerRotation_PlcRot_58(String playerRotationValue) {
        if(playerRotationValue != null) {
            this.playerRotationUnknown1[0] = playerRotationValue.substring(2, 4);
            this.playerRotationUnknown2[0] = playerRotationValue.substring(4, 6);
            this.playerRotationUnknown2[1] = playerRotationValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + playerRotationUnknown1[0];
        newScdDataByteString = newScdDataByteString + playerRotationUnknown2[0];
        newScdDataByteString = newScdDataByteString + playerRotationUnknown2[1];

        return newScdDataByteString;
    }
}
