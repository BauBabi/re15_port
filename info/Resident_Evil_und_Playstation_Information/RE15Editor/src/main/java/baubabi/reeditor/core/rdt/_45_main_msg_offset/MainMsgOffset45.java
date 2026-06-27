package baubabi.reeditor.core.rdt._45_main_msg_offset;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class MainMsgOffset45 {
    int mainMsgOffset;

    int rdtOffsetPositionStart;

    int rdtOffsetPositionEnd;

    int elementSize = 2;

    public MainMsgOffset45(int mainMsgOffset, int rdtOffsetPositionStart, int rdtOffsetPositionEnd) {
        this.mainMsgOffset = mainMsgOffset;
        this.rdtOffsetPositionStart = rdtOffsetPositionStart;
        this.rdtOffsetPositionEnd = rdtOffsetPositionEnd;
    }
}
