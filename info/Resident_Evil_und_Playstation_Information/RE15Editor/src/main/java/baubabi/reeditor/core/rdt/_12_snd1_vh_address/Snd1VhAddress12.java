package baubabi.reeditor.core.rdt._12_snd1_vh_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd1VhAddress12 {
    int address;

    int rdtOffsetPositionStart = 24;

    int rdtOffsetPositionEnd = 27;

    int elementSize = 4;

    public Snd1VhAddress12(int address) {
        this.address = address;
    }
}
