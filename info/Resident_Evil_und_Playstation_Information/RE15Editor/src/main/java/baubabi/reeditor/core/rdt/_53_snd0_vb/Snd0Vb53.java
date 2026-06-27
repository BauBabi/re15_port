package baubabi.reeditor.core.rdt._53_snd0_vb;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Snd0Vb53 {
    String snd0VbData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public Snd0Vb53(String snd0VbData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.snd0VbData = snd0VbData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
