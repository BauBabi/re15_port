package baubabi.reeditor.core.rdt._52_snd1_vh;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd1Vh52 {
    String snd1VhData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public Snd1Vh52(String snd1VhData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.snd1VhData = snd1VhData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
