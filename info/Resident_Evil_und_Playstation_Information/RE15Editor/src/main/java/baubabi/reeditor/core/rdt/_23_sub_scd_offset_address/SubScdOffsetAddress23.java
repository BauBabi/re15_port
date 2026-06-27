package baubabi.reeditor.core.rdt._23_sub_scd_offset_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class SubScdOffsetAddress23 {
    int address;

    int rdtOffsetPositionStart = 68;

    int rdtOffsetPositionEnd = 71;

    int elementSize = 4;

    public SubScdOffsetAddress23(int address) {
        this.address = address;
    }
}
