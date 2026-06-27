package baubabi.reeditor.core.rdt._04_door;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Door04 {
    int door;

    int rdtOffsetPositionStart = 4;

    int rdtOffsetPositionEnd = 4;

    int elementSize = 1;

    public Door04(int door) {
        this.door = door;
    }
}
