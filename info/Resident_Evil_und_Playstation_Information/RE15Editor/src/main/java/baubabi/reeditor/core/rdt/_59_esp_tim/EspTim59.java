package baubabi.reeditor.core.rdt._59_esp_tim;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class EspTim59 {
    String espTimData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public EspTim59(String espTimData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.espTimData = espTimData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
