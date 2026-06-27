package baubabi.reeditor.core.rdt._57_model_tim;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ModelTim57 {
    String modelTimData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public ModelTim57(String modelTimData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.modelTimData = modelTimData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
