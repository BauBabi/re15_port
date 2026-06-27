package baubabi.reeditor.core.rdt._46_main_msg;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class MainMsg46 {
    String mainMsgData;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 0;

    public MainMsg46(String mainMsgData, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.mainMsgData = mainMsgData;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
