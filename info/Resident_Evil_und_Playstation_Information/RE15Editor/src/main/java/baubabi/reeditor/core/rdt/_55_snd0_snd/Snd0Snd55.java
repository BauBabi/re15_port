package baubabi.reeditor.core.rdt._55_snd0_snd;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd0Snd55 {
    String snd0SndData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public Snd0Snd55(String snd0SndData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.snd0SndData = snd0SndData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
