package baubabi.reeditor.core.rdt._05_room_at;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class RoomAt05 {
    int roomAt;

    int rdtOffsetPositionStart = 5;

    int rdtOffsetPositionEnd = 5;

    int elementSize = 1;

    public RoomAt05(int roomAt) {
        this.roomAt = roomAt;
    }
}
