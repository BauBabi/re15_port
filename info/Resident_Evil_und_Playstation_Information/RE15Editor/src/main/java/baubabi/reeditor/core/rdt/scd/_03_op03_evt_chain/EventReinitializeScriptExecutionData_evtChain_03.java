package baubabi.reeditor.core.rdt.scd._03_op03_evt_chain;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class EventReinitializeScriptExecutionData_evtChain_03 {
    //4 Byte

    /*
        Reinitialize script execution data before executing new function ?
    */

    //1 byte
    private final String OPCODE = "03";

    //1 byte - unknown data
    private final String[] eventReinitializeScriptExecutionDataUnknown1 = new String[1];

    //1 byte - unknown data
    private final String[] eventReinitializeScriptExecutionDataUnknown2 = new String[1];

    //1 byte - event
    private final String[] eventReinitializeScriptExecutionDataEvent = new String[1];

    private final int size = 4;

    private final String displayName = "03 - reinitalize script execution";

    public EventReinitializeScriptExecutionData_evtChain_03(String eventReinitializeScriptExecutionDataValue) {
        if(eventReinitializeScriptExecutionDataValue != null) {
            this.eventReinitializeScriptExecutionDataUnknown1[0] = eventReinitializeScriptExecutionDataValue.substring(2, 4);
            this.eventReinitializeScriptExecutionDataUnknown2[0] = eventReinitializeScriptExecutionDataValue.substring(4, 6);
            this.eventReinitializeScriptExecutionDataEvent[0] = eventReinitializeScriptExecutionDataValue.substring(6, 8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + eventReinitializeScriptExecutionDataUnknown1[0];
        newScdDataByteString = newScdDataByteString + eventReinitializeScriptExecutionDataUnknown2[0];
        newScdDataByteString = newScdDataByteString + eventReinitializeScriptExecutionDataEvent[0];

        return newScdDataByteString;
    }
}
