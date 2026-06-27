package baubabi.reeditor.core.rdt._17_light_lit_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class LightLitAddress17 {
    int address;

    int rdtOffsetPositionStart = 44;

    int rdtOffsetPositionEnd = 47;

    int elementSize = 4;

    public LightLitAddress17(int address) {
        this.address = address;
    }
}
