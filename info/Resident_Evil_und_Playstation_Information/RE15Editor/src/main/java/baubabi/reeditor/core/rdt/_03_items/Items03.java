package baubabi.reeditor.core.rdt._03_items;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Items03 {
    int items;

    int rdtOffsetPositionStart = 3;

    int rdtOffsetPositionEnd = 3;

    int elementSize = 1;

    public Items03(int items) {
        this.items = items;
    }
}
