package baubabi.reeditor.core.rdt._38_floor_flr;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class FloorFlr38 {
    String floorFlrData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public FloorFlr38(String floorFlrData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.floorFlrData = floorFlrData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
