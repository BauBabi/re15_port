package baubabi.reeditor.core.rdt._02_amount_md1_room;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Md1Room02 {
    int md1;

    int rdtOffsetPositionStart = 2;

    int rdtOffsetPositionEnd = 2;

    int elementSize = 1;

    public Md1Room02(int md1) {
        this.md1 = md1;
    }
}
