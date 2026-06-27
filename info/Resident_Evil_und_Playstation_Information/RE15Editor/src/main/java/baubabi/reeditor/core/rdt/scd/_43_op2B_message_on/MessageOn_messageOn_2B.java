package baubabi.reeditor.core.rdt.scd._43_op2B_message_on;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class MessageOn_messageOn_2B {
    //6 Byte

    /*
        Specify the Message to be displayed
    */

    //1 byte
    private final String OPCODE = "2B"; /* 0x2b */

    //1 byte - unknown value
    private final String[] messageOnUnknown1 = new String[1];

    //1 byte - The message text number to be displayed
    private final String[] messageOnId = new String[1]; /* Text number to display (from offset 13 or 14 of RDT file) */

    //3 byte - unknown value 2 - Its an Array of [3] with 1 byte each
    private final String[] messageOnUnknown2 = new String[1];

    private final int size = 4;

    private final String displayName = "2B - message on";

    public MessageOn_messageOn_2B(String messageOnValue) {
        if(messageOnValue != null) {
            this.messageOnUnknown1[0] = messageOnValue.substring(2,4);
            this.messageOnId[0] = messageOnValue.substring(4,6);
            this.messageOnUnknown2[0] = messageOnValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + messageOnUnknown1[0];
        newScdDataByteString = newScdDataByteString + messageOnId[0];
        newScdDataByteString = newScdDataByteString + messageOnUnknown2[0];

        return newScdDataByteString;
    }
}
