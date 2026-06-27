package baubabi.reeditor.core.rdt._26_something_evtl_eff_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class SomethingEvtlEffAddress26 {
    int address;

    int rdtOffsetPositionStart = 80;

    int rdtOffsetPositionEnd = 83;

    int elementSize = 4;

    public SomethingEvtlEffAddress26(int address) {
        this.address = address;
    }
}
