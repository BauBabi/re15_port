package baubabi.reeditor.core.rdt._29_animation_rbj_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class AnimationRbjAddress29 {
    int address;

    int rdtOffsetPositionStart = 92;

    int rdtOffsetPositionEnd = 95;

    int elementSize = 4;

    public AnimationRbjAddress29(int address) {
        this.address = address;
    }
}
