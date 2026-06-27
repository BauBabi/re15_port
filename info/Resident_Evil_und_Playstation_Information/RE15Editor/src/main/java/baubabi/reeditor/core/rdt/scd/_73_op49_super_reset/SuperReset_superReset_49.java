package baubabi.reeditor.core.rdt.scd._73_op49_super_reset;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SuperReset_superReset_49 {
    //8 Byte

    /*
        Something, probably resolve the super data
    */

    //1 byte
    private final String OPCODE = "49";

    //7 byte - unknown data
    private final String[] superResetUnknown = new String[7];

    private final int size = 8;

    private final String displayName = OPCODE + " - super reset";

    public SuperReset_superReset_49(String superResetValue) {
        if(superResetValue != null) {
            this.superResetUnknown[0] = superResetValue.substring(2, 4);
            this.superResetUnknown[1] = superResetValue.substring(4, 6);
            this.superResetUnknown[2] = superResetValue.substring(6, 8);
            this.superResetUnknown[3] = superResetValue.substring(8, 10);
            this.superResetUnknown[4] = superResetValue.substring(10, 12);
            this.superResetUnknown[5] = superResetValue.substring(12, 14);
            this.superResetUnknown[6] = superResetValue.substring(14, 16);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + superResetUnknown[0];
        newScdDataByteString = newScdDataByteString + superResetUnknown[1];
        newScdDataByteString = newScdDataByteString + superResetUnknown[2];
        newScdDataByteString = newScdDataByteString + superResetUnknown[3];
        newScdDataByteString = newScdDataByteString + superResetUnknown[4];
        newScdDataByteString = newScdDataByteString + superResetUnknown[5];
        newScdDataByteString = newScdDataByteString + superResetUnknown[6];

        return newScdDataByteString;
    }
}
