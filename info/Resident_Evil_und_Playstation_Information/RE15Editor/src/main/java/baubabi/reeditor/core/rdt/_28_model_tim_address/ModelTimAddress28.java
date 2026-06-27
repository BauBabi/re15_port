package baubabi.reeditor.core.rdt._28_model_tim_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ModelTimAddress28 {
    int address;

    int rdtOffsetPositionStart = 88;

    int rdtOffsetPositionEnd = 91;

    int elementSize = 4;

    public ModelTimAddress28(int address) {
        this.address = address;
    }
}
