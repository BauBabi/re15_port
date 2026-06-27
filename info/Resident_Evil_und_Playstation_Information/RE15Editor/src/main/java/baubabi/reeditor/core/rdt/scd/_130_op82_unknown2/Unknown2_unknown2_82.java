package baubabi.reeditor.core.rdt.scd._130_op82_unknown2;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class Unknown2_unknown2_82 {
    //3 Byte

    /*
        Unknown data
    */

    //1 byte
    private final String OPCODE = "82";

    //2 byte - unknown data value
    private final String[] unknown2Value = new String[2];

    private final int size = 3;

    private final String displayName = OPCODE + " - unknown 2";

    public Unknown2_unknown2_82(String unknown2Value) {
        if(unknown2Value != null) {
            this.unknown2Value[0] = unknown2Value.substring(2, 4);
            this.unknown2Value[1] = unknown2Value.substring(4, 6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + unknown2Value[0];
        newScdDataByteString = newScdDataByteString + unknown2Value[1];

        return newScdDataByteString;
    }
}
