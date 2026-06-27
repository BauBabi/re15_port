package baubabi.reeditor.core.rdt.scd._58_op3A_sce_espr_on;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class AnimationAboveBackground_sceEsprOn_3A {
    //16 Byte

    /*
        2D animation above background (fire)
    */

    //1 byte
    private final String OPCODE = "3A";

    //15 byte - the 2D animation data
    private final String[] animationAboveBackgroundData = new String[15];

    private final int size = 16;

    private final String displayName = "3A - 2d animation above background";

    public AnimationAboveBackground_sceEsprOn_3A(String animationAboveBackgroundValue) {
        if(animationAboveBackgroundValue != null) {
            this.animationAboveBackgroundData[0] = animationAboveBackgroundValue.substring(2, 4);
            this.animationAboveBackgroundData[1] = animationAboveBackgroundValue.substring(4, 6);
            this.animationAboveBackgroundData[2] = animationAboveBackgroundValue.substring(6, 8);
            this.animationAboveBackgroundData[3] = animationAboveBackgroundValue.substring(8, 10);
            this.animationAboveBackgroundData[4] = animationAboveBackgroundValue.substring(10, 12);
            this.animationAboveBackgroundData[5] = animationAboveBackgroundValue.substring(12, 14);
            this.animationAboveBackgroundData[6] = animationAboveBackgroundValue.substring(14, 16);
            this.animationAboveBackgroundData[7] = animationAboveBackgroundValue.substring(16, 18);
            this.animationAboveBackgroundData[8] = animationAboveBackgroundValue.substring(18, 20);
            this.animationAboveBackgroundData[9] = animationAboveBackgroundValue.substring(20, 22);
            this.animationAboveBackgroundData[10] = animationAboveBackgroundValue.substring(22, 24);
            this.animationAboveBackgroundData[11] = animationAboveBackgroundValue.substring(24, 26);
            this.animationAboveBackgroundData[12] = animationAboveBackgroundValue.substring(26, 28);
            this.animationAboveBackgroundData[13] = animationAboveBackgroundValue.substring(28, 30);
            this.animationAboveBackgroundData[14] = animationAboveBackgroundValue.substring(30, 32);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[0];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[1];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[2];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[3];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[4];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[5];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[6];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[7];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[8];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[9];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[10];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[11];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[12];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[13];
        newScdDataByteString = newScdDataByteString + animationAboveBackgroundData[14];

        return newScdDataByteString;
    }
}
