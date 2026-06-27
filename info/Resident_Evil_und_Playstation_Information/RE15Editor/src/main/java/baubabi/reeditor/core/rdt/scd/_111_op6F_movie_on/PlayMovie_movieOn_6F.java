package baubabi.reeditor.core.rdt.scd._111_op6F_movie_on;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class PlayMovie_movieOn_6F {
    //2 Byte

    /*
        play movie
    */

    //1 byte
    private final String OPCODE = "6F"; /* 0x6f */

    //1 byte - id of movie to play
    private final String[] playMovieId = new String[1]; /* Movie to play */

    private final int size = 2;

    private final String displayName = OPCODE + " - play movie";

    public PlayMovie_movieOn_6F(String playMovieValue) {
        if(playMovieValue != null) {
            this.playMovieId[0] = playMovieValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + playMovieId[0];

        return newScdDataByteString;
    }
}
