package baubabi.reeditor.core.rdt.scd._99_op63_plc_gun_eff;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class GunEffect_plcGunEff_63 {
    //1 Byte

    /*
        Don't know - gun effect
    */

    //1 byte
    private final String OPCODE = "63";

    private final int size = 1;

    private final String displayName = OPCODE + " - gun effect";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
