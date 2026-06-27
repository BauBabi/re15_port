package baubabi.reeditor.core.rdt._19_floor_flr_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class FloorFlrAddress19 {
    int address;

    int rdtOffsetPositionStart = 52;

    int rdtOffsetPositionEnd = 55;

    int elementSize = 4;

    public FloorFlrAddress19(int address) {
        this.address = address;
    }
}
