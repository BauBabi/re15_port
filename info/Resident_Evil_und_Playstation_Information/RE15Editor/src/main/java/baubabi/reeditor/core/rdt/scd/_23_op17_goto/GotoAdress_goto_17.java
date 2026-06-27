package baubabi.reeditor.core.rdt.scd._23_op17_goto;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class GotoAdress_goto_17 {
    //6 Byte

    /*
        Jump to another instruction in current event.

          'rel_offset' is relative to start of this instruction.
    */

    //1 byte
    private final String OPCODE = "17";

    //1 byte - unknown value 1
    private final String[] gotoAdressUnknown1 = new String[1];

    //1 byte - unknown value 2
    private final String[] gotoAdressUnknown2 = new String[1];

    //1 byte - unknown value 3
    private final String[] gotoAdressUnknown3 = new String[1];

    //2 byte - the relative offset to go to
    private final String[] gotoAdressRelOffset = new String[2];

    private final int size = 6;

    private final String displayName = "17 - goto";

    public GotoAdress_goto_17(String gotoAdressValue) {
        if(gotoAdressValue != null) {
            this.gotoAdressUnknown1[0] = gotoAdressValue.substring(2,4);
            this.gotoAdressUnknown2[0] = gotoAdressValue.substring(4,6);
            this.gotoAdressUnknown3[0] = gotoAdressValue.substring(6,8);
            this.gotoAdressRelOffset[0] = gotoAdressValue.substring(8,10);
            this.gotoAdressRelOffset[1] = gotoAdressValue.substring(10,12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + gotoAdressUnknown1[0];
        newScdDataByteString = newScdDataByteString + gotoAdressUnknown2[0];
        newScdDataByteString = newScdDataByteString + gotoAdressUnknown3[0];
        newScdDataByteString = newScdDataByteString + gotoAdressRelOffset[0];
        newScdDataByteString = newScdDataByteString + gotoAdressRelOffset[1];

        return newScdDataByteString;
    }
}
