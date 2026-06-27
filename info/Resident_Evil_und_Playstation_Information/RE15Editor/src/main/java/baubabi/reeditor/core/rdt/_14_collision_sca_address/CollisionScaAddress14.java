package baubabi.reeditor.core.rdt._14_collision_sca_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class CollisionScaAddress14 {
    int address;

    int rdtOffsetPositionStart = 32;

    int rdtOffsetPositionEnd = 35;

    int elementSize = 4;

    public CollisionScaAddress14(int address) {
        this.address = address;
    }
}