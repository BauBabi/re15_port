package baubabi.reeditor.core.rdt._21_message_msg_address;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class MessageMsgAddress21 {
    int address;

    int rdtOffsetPositionStart = 60;

    int rdtOffsetPositionEnd = 63;

    int elementSize = 4;

    public MessageMsgAddress21(int address) {
        this.address = address;
    }
}
