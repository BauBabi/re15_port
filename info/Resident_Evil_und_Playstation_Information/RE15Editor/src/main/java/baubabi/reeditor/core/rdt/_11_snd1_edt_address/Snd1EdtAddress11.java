package baubabi.reeditor.core.rdt._11_snd1_edt_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd1EdtAddress11 {
    int address;

    int rdtOffsetPositionStart = 20;

    int rdtOffsetPositionEnd = 23;

    int elementSize = 4;

    public Snd1EdtAddress11(int address) {
        this.address = address;
    }
}
