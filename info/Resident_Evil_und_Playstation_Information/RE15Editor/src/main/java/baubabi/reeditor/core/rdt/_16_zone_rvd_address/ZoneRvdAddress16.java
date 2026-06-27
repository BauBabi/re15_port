package baubabi.reeditor.core.rdt._16_zone_rvd_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ZoneRvdAddress16 {
    int address;

    int rdtOffsetPositionStart = 40;

    int rdtOffsetPositionEnd = 43;

    int elementSize = 4;

    public ZoneRvdAddress16(int address) {
        this.address = address;
    }
}
