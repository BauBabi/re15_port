package baubabi.reeditor.core.rdt.scd._47_op2F_speed_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetInternalRegister_speedSet_2F {
    //4 Byte

    /*
        This instruction sets one of the words of an internal register (composed of 3 words).
    */

    //1 byte
    private final String OPCODE = "2F"; /* 0x2f */

    //1 byte - number of words
    private final String[] setInternalRegisterComponent = new String[1]; /* 0,1,2 number of word  */

    //2 byte - a value to set for component n
    private final String[] setInternalRegisterValue = new String[2]; /* Value to set for n-th component */

    private final int size = 4;

    private final String displayName = "2F - set internal register speed";

    public SetInternalRegister_speedSet_2F(String setInternalRegisterValue) {
        if(setInternalRegisterValue != null) {
            this.setInternalRegisterComponent[0] = setInternalRegisterValue.substring(2,4);
            this.setInternalRegisterValue[0] = setInternalRegisterValue.substring(4,6);
            this.setInternalRegisterValue[1] = setInternalRegisterValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setInternalRegisterComponent[0];
        newScdDataByteString = newScdDataByteString + setInternalRegisterValue[0];
        newScdDataByteString = newScdDataByteString + setInternalRegisterValue[1];

        return newScdDataByteString;
    }
}
