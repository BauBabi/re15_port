package baubabi.reeditor.core.rdt._62_snd0_snd_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd0SndAddress62 {
    int address;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public Snd0SndAddress62(int address, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.address = address;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
