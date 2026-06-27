package baubabi.reeditor.core.rdt._49_snd0_edt;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd0Edt49 {
    String snd0EdtData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public Snd0Edt49(String snd0EdtData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.snd0EdtData = snd0EdtData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
