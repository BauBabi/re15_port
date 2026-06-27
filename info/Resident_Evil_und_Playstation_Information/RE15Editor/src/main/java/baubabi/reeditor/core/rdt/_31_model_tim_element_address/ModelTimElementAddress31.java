package baubabi.reeditor.core.rdt._31_model_tim_element_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ModelTimElementAddress31 {
    int address;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 4;

    public ModelTimElementAddress31(int address, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.address = address;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
