package baubabi.reeditor.core.rdt.scd._48_op30_add_speed;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetInternalRegister_addSpeed_30 {
    //1 Byte

    /*
         Sets internal register from an internal memory location.
    */

    //1 byte
    private final String OPCODE = "30";

    private final int size = 1;

    private final String displayName = "30 - set internal register add speed";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
