package baubabi.reeditor.core.rdt._65_esp_tim_element_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class EspTimElementAddress65 {
    int address;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public EspTimElementAddress65(int address, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.address = address;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
