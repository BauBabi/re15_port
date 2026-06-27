package baubabi.reeditor.core.rdt._54_snd1_vb;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd1Vb54 {
    String snd1VbData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public Snd1Vb54(String snd1VbData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.snd1VbData = snd1VbData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
