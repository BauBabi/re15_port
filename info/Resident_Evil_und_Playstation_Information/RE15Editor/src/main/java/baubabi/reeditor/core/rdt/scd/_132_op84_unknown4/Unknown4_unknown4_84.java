package baubabi.reeditor.core.rdt.scd._132_op84_unknown4;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Unknown4_unknown4_84 {
    //2 Byte

    /*
        Unknown data
    */

    //1 byte
    private final String OPCODE = "84";

    //1 byte - unknown data value
    private final String[] unknown4Value = new String[2];

    private final int size = 2;

    private final String displayName = OPCODE + " - unknown 4";

    public Unknown4_unknown4_84(String unknown4Value) {
        if(unknown4Value != null) {
            this.unknown4Value[0] = unknown4Value.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + unknown4Value[0];

        return newScdDataByteString;
    }
}
