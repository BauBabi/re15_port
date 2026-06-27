package baubabi.reeditor.core.rdt._34_light_lit;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class LightLit34 {
    String lightLitData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public LightLit34(String lightLitData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.lightLitData = lightLitData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
