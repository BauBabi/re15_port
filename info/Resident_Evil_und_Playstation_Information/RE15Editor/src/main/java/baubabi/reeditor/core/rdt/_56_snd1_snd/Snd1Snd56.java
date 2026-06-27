package baubabi.reeditor.core.rdt._56_snd1_snd;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd1Snd56 {
    String snd1SndData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public Snd1Snd56(String snd1SndData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.snd1SndData = snd1SndData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
