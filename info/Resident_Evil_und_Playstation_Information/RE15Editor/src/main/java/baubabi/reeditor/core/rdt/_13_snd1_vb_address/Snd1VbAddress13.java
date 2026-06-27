package baubabi.reeditor.core.rdt._13_snd1_vb_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd1VbAddress13 {
    int address;

    int rdtOffsetPositionStart = 28;

    int rdtOffsetPositionEnd = 31;

    int elementSize = 4;

    public Snd1VbAddress13(int address) {
        this.address = address;
    }
}
