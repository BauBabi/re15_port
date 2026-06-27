package baubabi.reeditor.core.rdt._32_model_md1_element_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ModelMd1ElementAddress32 {
    int address;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 4;

    public ModelMd1ElementAddress32(int address, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.address = address;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
