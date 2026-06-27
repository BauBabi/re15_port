package baubabi.reeditor.core.rdt._27_esp_tim_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class EspTimAddress27 {
    int address;

    int rdtOffsetPositionStart = 84;

    int rdtOffsetPositionEnd = 87;

    int elementSize = 4;

    public EspTimAddress27(int address) {
        this.address = address;
    }
}
