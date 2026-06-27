package baubabi.reeditor.core.rdt._43_extra_scd_offset;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ExtraScdOffset43 {
    int correspondingScdOffset;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 2;

    public ExtraScdOffset43(int correspondingScdOffset, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.correspondingScdOffset = correspondingScdOffset;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
