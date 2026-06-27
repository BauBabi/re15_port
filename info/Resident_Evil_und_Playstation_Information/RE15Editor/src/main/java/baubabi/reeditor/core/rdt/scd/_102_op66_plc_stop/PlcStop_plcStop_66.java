package baubabi.reeditor.core.rdt.scd._102_op66_plc_stop;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class PlcStop_plcStop_66 {
    //1 Byte

    /*
        Don't know - stops character movement
    */

    //1 byte
    private final String OPCODE = "66";

    private final int size = 1;

    private final String displayName = OPCODE + " - plc stop";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
