package baubabi.reeditor.core.rdt._33_zone_rvd;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ZoneRvd33 {
    String zoneRvdData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public ZoneRvd33(String zoneRvdData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.zoneRvdData = zoneRvdData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
