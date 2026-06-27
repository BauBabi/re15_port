package baubabi.reeditor.core.rdt.scd._133_op85_unknown5;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class UnknownSomethingOffset6_unknown5_85 {
    //6 Byte

    /*
        Related to offset 6 of RDT file
    */

    //1 byte
    private final String OPCODE = "85";

    //5 byte - the data for whatever it has to do with offset 6
    private final String[] unknownSomethingOffset6Data = new String[5];

    private final int size = 6;

    private final String displayName = OPCODE + " - unknown 5";

    public UnknownSomethingOffset6_unknown5_85(String unknownSomethingOffset6Value) {
        if(unknownSomethingOffset6Value != null) {
            this.unknownSomethingOffset6Data[0] = unknownSomethingOffset6Value.substring(2, 4);
            this.unknownSomethingOffset6Data[1] = unknownSomethingOffset6Value.substring(4, 6);
            this.unknownSomethingOffset6Data[2] = unknownSomethingOffset6Value.substring(6, 8);
            this.unknownSomethingOffset6Data[3] = unknownSomethingOffset6Value.substring(8, 10);
            this.unknownSomethingOffset6Data[4] = unknownSomethingOffset6Value.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + unknownSomethingOffset6Data[0];
        newScdDataByteString = newScdDataByteString + unknownSomethingOffset6Data[1];
        newScdDataByteString = newScdDataByteString + unknownSomethingOffset6Data[2];
        newScdDataByteString = newScdDataByteString + unknownSomethingOffset6Data[3];
        newScdDataByteString = newScdDataByteString + unknownSomethingOffset6Data[4];

        return newScdDataByteString;
    }
}
