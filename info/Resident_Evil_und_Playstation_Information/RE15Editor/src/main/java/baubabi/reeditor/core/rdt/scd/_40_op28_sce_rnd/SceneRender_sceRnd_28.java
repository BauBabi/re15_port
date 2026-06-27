package baubabi.reeditor.core.rdt.scd._40_op28_sce_rnd;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SceneRender_sceRnd_28 {
    //1 Byte

    /*
        Maybe something with scene Rendering?
    */

    //1 byte
    private final String OPCODE = "28";

    private final int size = 1;

    private final String displayName = "28 - render scene";

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;

        return newScdDataByteString;
    }
}
