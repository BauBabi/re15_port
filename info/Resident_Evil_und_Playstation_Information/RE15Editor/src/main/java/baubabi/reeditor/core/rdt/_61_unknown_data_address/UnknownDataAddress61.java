package baubabi.reeditor.core.rdt._61_unknown_data_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class UnknownDataAddress61 {
    int address;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public UnknownDataAddress61(int address, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.address = address;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
