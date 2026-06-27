package baubabi.reeditor.core.rdt.scd._57_op39_dir_ck;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class DirectoryCk_dirCk_39 {
    //8 Byte

    /*
        Sets directory ck
    */

    //1 byte
    private final String OPCODE = "39";

    //3 byte - directory ck data
    private final String[] directoryCkData = new String[3];

    private final int size = 4;

    private final String displayName = "39 - set directory ck";

    public DirectoryCk_dirCk_39(String directoryCkValue) {
        if(directoryCkValue != null) {
            this.directoryCkData[0] = directoryCkValue.substring(2, 4);
            this.directoryCkData[1] = directoryCkValue.substring(4, 6);
            this.directoryCkData[2] = directoryCkValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + directoryCkData[0];
        newScdDataByteString = newScdDataByteString + directoryCkData[1];
        newScdDataByteString = newScdDataByteString + directoryCkData[2];

        return newScdDataByteString;
    }
}
