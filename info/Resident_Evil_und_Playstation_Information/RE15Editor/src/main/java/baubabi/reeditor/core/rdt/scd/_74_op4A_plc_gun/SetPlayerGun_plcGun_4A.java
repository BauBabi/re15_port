package baubabi.reeditor.core.rdt.scd._74_op4A_plc_gun;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetPlayerGun_plcGun_4A {
    //2 Byte

    /*
        Maybe the gun of the player
    */

    //1 byte
    private final String OPCODE = "4A";

    //1 byte - unknown data. Maybe the gun?
    private final String[] setPlayerGunUnknown = new String[1];

    private final int size = 2;

    private final String displayName = OPCODE + " - set player gun";

    public SetPlayerGun_plcGun_4A(String setPlayerGunValue) {
        if(setPlayerGunValue != null) {
            this.setPlayerGunUnknown[0] = setPlayerGunValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setPlayerGunUnknown[0];

        return newScdDataByteString;
    }
}
