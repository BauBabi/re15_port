package baubabi.reeditor.core.rdt._40_main_scd;

import baubabi.reeditor.core.rdt._22_main_scd_offset_address.MainScdOffsetAddress22;
import baubabi.reeditor.core.rdt._39_main_scd_offset.MainScdOffset39;
import lombok.Getter;
import lombok.Setter;

import java.util.ArrayList;
import java.util.List;

@Getter
@Setter
public class MainScd40 {
    String scdData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    List<Object> singleScdDataElements;

    public MainScd40(String scdData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.scdData = scdData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
        this.singleScdDataElements = new ArrayList<>();
    }

    public void setSingleScdDataElements(List<Object> singleScdDataElementsList) {
        this.singleScdDataElements = singleScdDataElementsList;
    }

    public void setScdData(String newMainScdData) {
        this.scdData = newMainScdData;
    }

    public Object getParentScdOffsetElement() {
        return MainScdOffset39.class;
    }

    public Object getScdAddressElement() { return MainScdOffsetAddress22.class; }
}
