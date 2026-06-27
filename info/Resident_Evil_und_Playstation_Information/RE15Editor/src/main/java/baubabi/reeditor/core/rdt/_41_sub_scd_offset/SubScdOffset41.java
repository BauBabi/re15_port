package baubabi.reeditor.core.rdt._41_sub_scd_offset;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class SubScdOffset41 {
    int correspondingScdOffset;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 2;

    public SubScdOffset41(int correspondingScdOffset, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.correspondingScdOffset = correspondingScdOffset;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
