package baubabi.reeditor.core.rdt._24_extra_scd_offset_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ExtraScdOffsetAddress24 {
    int address;

    int rdtOffsetPositionStart = 72;

    int rdtOffsetPositionEnd = 75;

    int elementSize = 4;

    public ExtraScdOffsetAddress24(int address) {
        this.address = address;
    }
}
