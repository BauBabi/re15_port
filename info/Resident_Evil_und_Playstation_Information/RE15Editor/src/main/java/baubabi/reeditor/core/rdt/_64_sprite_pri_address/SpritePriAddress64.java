package baubabi.reeditor.core.rdt._64_sprite_pri_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class SpritePriAddress64 {
    int address;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public SpritePriAddress64(int address, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.address = address;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
