package baubabi.reeditor.core.rdt._60_unknown_data;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class UnknownData60 {
    String unknownData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public UnknownData60(String unknownData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.unknownData = unknownData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
