package baubabi.reeditor.core.rdt._44_extra_scd;

import baubabi.reeditor.core.rdt._24_extra_scd_offset_address.ExtraScdOffsetAddress24;
import baubabi.reeditor.core.rdt._43_extra_scd_offset.ExtraScdOffset43;
import lombok.Getter;
import lombok.Setter;

import java.util.ArrayList;
import java.util.List;

@Getter
@Setter
public class ExtraScd44 {
    String scdData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    List<Object> singleScdDataElements;

    public ExtraScd44(String scdData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.scdData = scdData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
        this.singleScdDataElements = new ArrayList<>();
    }

    public void setSingleScdDataElements(List<Object> singleScdDataElementsList) {
        this.singleScdDataElements = singleScdDataElementsList;
    }

    public void setScdData(String newExtraScdData) {
        this.scdData = newExtraScdData;
    }

    public Object getParentScdOffsetElement() {
        return ExtraScdOffset43.class;
    }

    public Object getScdAddressElement() { return ExtraScdOffsetAddress24.class; }
}
