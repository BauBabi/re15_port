package baubabi.reeditor.core.rdt.scd._70_op46_aot_reset;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ActionPlayerTriggers_aotReset_46 {
    //10 byte

    /*
        Seems to define the action to do when the player triggers an event.
    */

    //1 byte
    private final String OPCODE = "46"; /* 0x46 */

    //1 byte - index of room object list
    private final String[] actionPlayerTriggersIndex = new String[1]; /* Index in list of room objects (doors, items, etc) */

    //2 byte - unknown value 1 - array of 2 with 1 byte each
    private final String[] actionPlayerTriggersUnknown1 = new String[2];

    //2 byte - unknown value 2 - array of 3 with 2 byte each
    private final String[] actionPlayerTriggersUnknown2 = new String[6];

    private final int size = 10;

    private final String displayName = OPCODE + " - action when player trigger event";

    public ActionPlayerTriggers_aotReset_46(String actionPlayerTriggersValue) {
        if(actionPlayerTriggersValue != null) {
            this.actionPlayerTriggersIndex[0] = actionPlayerTriggersValue.substring(2,4);
            this.actionPlayerTriggersUnknown1[0] = actionPlayerTriggersValue.substring(4,6);
            this.actionPlayerTriggersUnknown1[1] = actionPlayerTriggersValue.substring(6,8);
            this.actionPlayerTriggersUnknown2[0] = actionPlayerTriggersValue.substring(8,10);
            this.actionPlayerTriggersUnknown2[1] = actionPlayerTriggersValue.substring(10,12);
            this.actionPlayerTriggersUnknown2[2] = actionPlayerTriggersValue.substring(12,14);
            this.actionPlayerTriggersUnknown2[3] = actionPlayerTriggersValue.substring(14,16);
            this.actionPlayerTriggersUnknown2[4] = actionPlayerTriggersValue.substring(16,18);
            this.actionPlayerTriggersUnknown2[5] = actionPlayerTriggersValue.substring(18,20);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + actionPlayerTriggersIndex[0];
        newScdDataByteString = newScdDataByteString + actionPlayerTriggersUnknown1[0];
        newScdDataByteString = newScdDataByteString + actionPlayerTriggersUnknown1[1];
        newScdDataByteString = newScdDataByteString + actionPlayerTriggersUnknown2[0];
        newScdDataByteString = newScdDataByteString + actionPlayerTriggersUnknown2[1];
        newScdDataByteString = newScdDataByteString + actionPlayerTriggersUnknown2[2];
        newScdDataByteString = newScdDataByteString + actionPlayerTriggersUnknown2[3];
        newScdDataByteString = newScdDataByteString + actionPlayerTriggersUnknown2[4];
        newScdDataByteString = newScdDataByteString + actionPlayerTriggersUnknown2[5];

        return newScdDataByteString;
    }
}
