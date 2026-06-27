package baubabi.reeditor.core.rdt._01_cut_amount_cameras_room;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Cut01 {

    int cut;

    int rdtOffsetPositionStart = 1;

    int rdtOffsetPositionEnd = 1;

    int elementSize = 1;

    public Cut01(int cut) {
        this.cut = cut;
    }
}
