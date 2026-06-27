package baubabi.reeditor.core.rdt.scd._143_unknown_some_weird_return;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class UnknownSomeWeirdReturn_weirdReturn_FF {
    //2 Byte

    /*
        Some weird return value FF00
    */

    //1 byte
    private final String OPCODE = "FF";

    //1 byte return value
    private final String[] unknownSomeWeirdReturnValue = new String[1];

    private final int size = 2;

    private final String displayName = OPCODE + " - unknown some weird return";

    public UnknownSomeWeirdReturn_weirdReturn_FF(String unknownSomeWeirdReturnValue) {
        if(unknownSomeWeirdReturnValue != null) {
            this.unknownSomeWeirdReturnValue[0] = unknownSomeWeirdReturnValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + unknownSomeWeirdReturnValue[0];

        return newScdDataByteString;
    }
}
