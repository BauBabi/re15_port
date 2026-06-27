package baubabi.reeditor.core.rdt._06_reverb_lv;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ReverbLv06 {
    int reverbLv;

    int rdtOffsetPositionStart = 6;

    int rdtOffsetPositionEnd = 6;

    int elementSize = 1;

    public ReverbLv06(int reverbLv) {
        this.reverbLv = reverbLv;
    }
}
