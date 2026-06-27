package baubabi.reeditor.core.rdt.scd._115_op73_mirror_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class MirrorSet_mirrorSet_73 {
    //8 Byte

    /*
        Don't know the function of it
    */

    //1 byte
    private final String OPCODE = "73";

    //7 byte - the mirror_set data
    private final String[] mirrorSetData = new String[7];

    private final int size = 8;

    private final String displayName = OPCODE + " - mirror set";

    public MirrorSet_mirrorSet_73(String mirrorSetValue) {
        if(mirrorSetValue != null) {
            this.mirrorSetData[0] = mirrorSetValue.substring(2, 4);
            this.mirrorSetData[1] = mirrorSetValue.substring(4, 6);
            this.mirrorSetData[2] = mirrorSetValue.substring(6, 8);
            this.mirrorSetData[3] = mirrorSetValue.substring(8, 10);
            this.mirrorSetData[4] = mirrorSetValue.substring(10, 12);
            this.mirrorSetData[5] = mirrorSetValue.substring(12, 14);
            this.mirrorSetData[6] = mirrorSetValue.substring(14, 16);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + mirrorSetData[0];
        newScdDataByteString = newScdDataByteString + mirrorSetData[1];
        newScdDataByteString = newScdDataByteString + mirrorSetData[2];
        newScdDataByteString = newScdDataByteString + mirrorSetData[3];
        newScdDataByteString = newScdDataByteString + mirrorSetData[4];
        newScdDataByteString = newScdDataByteString + mirrorSetData[5];
        newScdDataByteString = newScdDataByteString + mirrorSetData[6];

        return newScdDataByteString;
    }
}
