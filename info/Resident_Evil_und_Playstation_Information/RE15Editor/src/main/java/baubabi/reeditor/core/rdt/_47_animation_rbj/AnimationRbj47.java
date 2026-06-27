package baubabi.reeditor.core.rdt._47_animation_rbj;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class AnimationRbj47 {
    String animationRbjData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public AnimationRbj47(String animationRbjData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.animationRbjData = animationRbjData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
