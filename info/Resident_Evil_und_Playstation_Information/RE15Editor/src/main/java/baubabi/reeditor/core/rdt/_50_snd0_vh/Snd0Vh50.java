package baubabi.reeditor.core.rdt._50_snd0_vh;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd0Vh50 {
    String snd0VhData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public Snd0Vh50(String snd0VhData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.snd0VhData = snd0VhData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
