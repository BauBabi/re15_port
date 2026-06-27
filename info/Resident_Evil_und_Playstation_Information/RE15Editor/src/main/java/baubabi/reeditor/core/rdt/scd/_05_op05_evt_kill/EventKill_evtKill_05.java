package baubabi.reeditor.core.rdt.scd._05_op05_evt_kill;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class EventKill_evtKill_05 {
    //2 Byte

    /*
        MAYBE* Disable an event.
    */

    //1 byte
    private final String OPCODE = "05";

    //1 byte - event value
    private final String[] eventKillEvent = new String[1];

    private final int size = 2;

    private final String displayName = "05 - disable event";

    public EventKill_evtKill_05(String eventKillValue) {
        if(eventKillValue != null) {this.eventKillEvent[0] = eventKillValue.substring(2,4);}
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + eventKillEvent[0];

        return newScdDataByteString;
    }
}
