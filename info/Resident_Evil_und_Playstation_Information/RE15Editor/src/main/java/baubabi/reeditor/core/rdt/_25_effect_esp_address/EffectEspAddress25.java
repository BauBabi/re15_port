package baubabi.reeditor.core.rdt._25_effect_esp_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class EffectEspAddress25 {
    int address;

    int rdtOffsetPositionStart = 76;

    int rdtOffsetPositionEnd = 79;

    int elementSize = 4;

    public EffectEspAddress25(int address) {
        this.address = address;
    }
}
