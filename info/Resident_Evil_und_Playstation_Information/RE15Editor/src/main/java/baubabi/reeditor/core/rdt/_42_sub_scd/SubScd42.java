package baubabi.reeditor.core.rdt._42_sub_scd;

import baubabi.reeditor.core.rdt._23_sub_scd_offset_address.SubScdOffsetAddress23;
import baubabi.reeditor.core.rdt._41_sub_scd_offset.SubScdOffset41;
import lombok.Getter;
import lombok.Setter;

import java.util.ArrayList;
import java.util.List;

@Getter
@Setter
public class SubScd42 {
    String scdData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    List<Object> singleScdDataElements;

    public SubScd42(String scdData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.scdData = scdData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
        this.singleScdDataElements = new ArrayList<>();
    }

    public void setSingleScdDataElements(List<Object> singleScdDataElementsList) {
        this.singleScdDataElements = singleScdDataElementsList;
    }
    public void setScdData(String newSubScdData) {
        this.scdData = newSubScdData;
    }

    public Object getParentScdOffsetElement() {
        return SubScdOffset41.class;
    }

    public Object getScdAddressElement() { return SubScdOffsetAddress23.class; }
}
