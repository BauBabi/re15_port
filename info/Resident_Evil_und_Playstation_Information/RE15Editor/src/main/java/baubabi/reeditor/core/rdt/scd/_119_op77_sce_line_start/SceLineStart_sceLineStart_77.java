package baubabi.reeditor.core.rdt.scd._119_op77_sce_line_start;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceLineStart_sceLineStart_77 {
    //4 Byte

    /*
        Don't know the function of it
    */

    //1 byte
    private final String OPCODE = "77";

    //3 byte - the Sce_line_start data
    private final String[] sceLineStartData = new String[3];

    private final int size = 4;

    private final String displayName = OPCODE + " - sce line start";

    public SceLineStart_sceLineStart_77(String sceLineStartValue) {
        if(sceLineStartValue != null) {
            this.sceLineStartData[0] = sceLineStartValue.substring(2, 4);
            this.sceLineStartData[1] = sceLineStartValue.substring(4, 6);
            this.sceLineStartData[2] = sceLineStartValue.substring(6, 8);
        }
    }
    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceLineStartData[0];
        newScdDataByteString = newScdDataByteString + sceLineStartData[1];
        newScdDataByteString = newScdDataByteString + sceLineStartData[2];

        return newScdDataByteString;
    }
}
