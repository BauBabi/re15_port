package baubabi.reeditor.core.rdt._63_snd1_snd_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd1SndAddress63 {
    int address;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public Snd1SndAddress63(int address, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.address = address;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
