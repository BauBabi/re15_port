package baubabi.reeditor.core.rdt._58_effect_esp;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class EffectEsp58 {
    String effectEspData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public EffectEsp58(String effectEspData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.effectEspData = effectEspData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
