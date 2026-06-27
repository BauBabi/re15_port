package baubabi.reeditor.core.rdt._36_collision_sca;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class CollisionSca36 {
    String collisionScaData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public CollisionSca36(String collisionScaData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.collisionScaData = collisionScaData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
