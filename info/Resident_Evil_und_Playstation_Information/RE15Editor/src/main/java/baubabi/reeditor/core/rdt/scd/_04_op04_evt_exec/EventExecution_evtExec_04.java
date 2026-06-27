package baubabi.reeditor.core.rdt.scd._04_op04_evt_exec;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class EventExecution_evtExec_04 {
    //4 Byte

    /*
      This instruction will execute the embedded two-byte instruction if the condition is true.
      When condition is 0xff, instruction is executed whenever there is a camera switch.
      Several instructions can be defined to be called when condition is true.
    */

    //1 byte
    private final String OPCODE = "04";

    //1 byte - condition to execute event
    private final String[] eventExecutionCondition = new String[1];

    //1 byte - Activate Opcode to execute
    private final String[] eventExecutionExecutionActivateOpcode = new String[1];

    //1 Byte - event to execute when condition is true
    private final String[] eventExecutionEvent = new String[1];

    private final int size = 4;

    private final String displayName = "04 - execute event";

    public EventExecution_evtExec_04(String eventExecutionValue) {
        if(eventExecutionValue != null) {
            this.eventExecutionCondition[0] = eventExecutionValue.substring(2, 4);
            this.eventExecutionExecutionActivateOpcode[0] = eventExecutionValue.substring(4, 6);
            this.eventExecutionEvent[0] = eventExecutionValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + eventExecutionCondition[0];
        newScdDataByteString = newScdDataByteString + eventExecutionExecutionActivateOpcode[0];
        newScdDataByteString = newScdDataByteString + eventExecutionEvent[0];

        return newScdDataByteString;
    }
}
