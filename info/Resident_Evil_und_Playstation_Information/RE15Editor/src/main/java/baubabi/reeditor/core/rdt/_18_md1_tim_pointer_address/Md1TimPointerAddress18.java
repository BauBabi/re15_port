package baubabi.reeditor.core.rdt._18_md1_tim_pointer_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class Md1TimPointerAddress18 {
    int address;

    int rdtOffsetPositionStart = 48;

    int rdtOffsetPositionEnd = 51;

    int elementSize = 4;

    public Md1TimPointerAddress18(int address) {
        this.address = address;
    }
}
