package baubabi.reeditor.core.rdt._22_main_scd_offset_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class MainScdOffsetAddress22 {
    int address;

    int rdtOffsetPositionStart = 64;

    int rdtOffsetPositionEnd = 67;

    int elementSize = 4;

    public MainScdOffsetAddress22(int address) {
        this.address = address;
    }
}
