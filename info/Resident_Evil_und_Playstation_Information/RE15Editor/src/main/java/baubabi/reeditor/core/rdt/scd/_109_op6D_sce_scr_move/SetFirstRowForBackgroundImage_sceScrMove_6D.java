package baubabi.reeditor.core.rdt.scd._109_op6D_sce_scr_move;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SetFirstRowForBackgroundImage_sceScrMove_6D {
    //4 Byte

    /*
        Set first row of screen to draw background images (Used for scrolling in room2000).
    */

    //1 byte
    private final String OPCODE = "6D"; /* 0x6d */

    //1 byte - a dummy value for it
    private final String[] setFirstRowForBackgroundImageDummy = new String[1];

    //2 byte - the new value for the starting row
    private final String[] setFirstRowForBackgroundImageNewStartRowValue = new String[2]; /* New value for starting row */

    private final int size = 4;

    private final String displayName = OPCODE + " - set first row to draw background";

    public SetFirstRowForBackgroundImage_sceScrMove_6D(String setFirstRowForBackgroundImageValue) {
        if(setFirstRowForBackgroundImageValue != null) {
            this.setFirstRowForBackgroundImageDummy[0] = setFirstRowForBackgroundImageValue.substring(2, 4);
            this.setFirstRowForBackgroundImageNewStartRowValue[0] = setFirstRowForBackgroundImageValue.substring(4, 6);
            this.setFirstRowForBackgroundImageNewStartRowValue[1] = setFirstRowForBackgroundImageValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + setFirstRowForBackgroundImageDummy[0];
        newScdDataByteString = newScdDataByteString + setFirstRowForBackgroundImageNewStartRowValue[0];
        newScdDataByteString = newScdDataByteString + setFirstRowForBackgroundImageNewStartRowValue[1];

        return newScdDataByteString;
    }
}
