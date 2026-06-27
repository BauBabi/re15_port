package baubabi.reeditor.core.rdt._09_snd0_vh_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd0VhAddress09 {
    int address;

    int rdtOffsetPositionStart = 12;

    int rdtOffsetPositionEnd = 15;

    int elementSize = 4;

    public Snd0VhAddress09(int address) {
        this.address = address;
    }
}
