package baubabi.reeditor.core.rdt._39_main_scd_offset;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class MainScdOffset39 {
    int correspondingScdOffset;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 2;

    public MainScdOffset39(int correspondingScdOffset, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.correspondingScdOffset = correspondingScdOffset;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
