package baubabi.reeditor.core.rdt._08_snd0_edt_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd0EdtAddress08 {
    int address;

    int rdtOffsetPositionStart = 8;

    int rdtOffsetPositionEnd = 11;

    int elementSize = 4;

    public Snd0EdtAddress08(int address) {
        this.address = address;
    }
}
