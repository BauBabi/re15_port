package baubabi.reeditor.core.rdt.scd._120_op78_sce_line_main;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceLineMain_sceLineMain_78 {
    //6 Byte

    /*
        Don't know the function of it
    */

    //1 byte
    private final String OPCODE = "78";

    //5 byte - the Sce_line_main data
    private final String[] sceLineMainData = new String[5];

    private final int size = 6;

    private final String displayName = OPCODE + " - sce line main";

    public SceLineMain_sceLineMain_78(String sceLineMainValue) {
        if(sceLineMainValue != null) {
            this.sceLineMainData[0] = sceLineMainValue.substring(2, 4);
            this.sceLineMainData[1] = sceLineMainValue.substring(4, 6);
            this.sceLineMainData[2] = sceLineMainValue.substring(6, 8);
            this.sceLineMainData[3] = sceLineMainValue.substring(8, 10);
            this.sceLineMainData[4] = sceLineMainValue.substring(10, 12);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + sceLineMainData[0];
        newScdDataByteString = newScdDataByteString + sceLineMainData[1];
        newScdDataByteString = newScdDataByteString + sceLineMainData[2];
        newScdDataByteString = newScdDataByteString + sceLineMainData[3];
        newScdDataByteString = newScdDataByteString + sceLineMainData[4];

        return newScdDataByteString;
    }
}
