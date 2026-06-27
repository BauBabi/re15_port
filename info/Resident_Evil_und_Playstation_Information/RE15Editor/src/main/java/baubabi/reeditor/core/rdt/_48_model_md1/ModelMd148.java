package baubabi.reeditor.core.rdt._48_model_md1;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ModelMd148 {
    String modelMd1Data;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public ModelMd148(String modelMd1Data, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.modelMd1Data = modelMd1Data;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
