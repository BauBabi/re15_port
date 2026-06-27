package baubabi.reeditor.core.rdt.scd._89_op59_xa_on;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class PlaySound_xaOn_59 {
    //4 Byte

    /*
        Play sound (character voice for example).
    */

    //1 byte
    private final String OPCODE = "59"; /* 0x59 */

    //1 byte - sound channel for playing the sound
    private final String[] playSoundChannel = new String[1]; /* channel on which to play sound ? */

    //2 byte - id of sound to play
    private final String[] playSoundId = new String[2]; /* ID of sound to play, i.e. file PLn/VOICE/STAGEn/Vxxx.SAP, with id=xxx */

    private final int size = 4;

    private final String displayName = OPCODE + " - play sound";

    public PlaySound_xaOn_59(String playSoundValue) {
        if(playSoundValue != null) {
            this.playSoundChannel[0] = playSoundValue.substring(2, 4);
            this.playSoundId[0] = playSoundValue.substring(4, 6);
            this.playSoundId[1] = playSoundValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + playSoundChannel[0];
        newScdDataByteString = newScdDataByteString + playSoundId[0];
        newScdDataByteString = newScdDataByteString + playSoundId[1];

        return newScdDataByteString;
    }
}
