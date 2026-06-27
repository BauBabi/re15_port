package baubabi.reeditor.core.rdt._51_snd1_edt;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd1Edt51 {
    String snd1EdtData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public Snd1Edt51(String snd1EdtData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.snd1EdtData = snd1EdtData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
