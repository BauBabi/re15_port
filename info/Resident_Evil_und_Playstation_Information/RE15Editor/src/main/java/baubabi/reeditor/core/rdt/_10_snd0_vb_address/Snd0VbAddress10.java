package baubabi.reeditor.core.rdt._10_snd0_vb_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd0VbAddress10 {
    int address;

    int rdtOffsetPositionStart = 16;

    int rdtOffsetPositionEnd = 19;

    int elementSize = 4;

    public Snd0VbAddress10(int address) {
        this.address = address;
    }
}
