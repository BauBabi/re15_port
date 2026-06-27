package baubabi.reeditor.core.rdt.scd._01_op01_evt_end;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ExitEvtAndReturn_evtEnd_01 {
    //2 Byte

    /*
 	     Exit current function and return a value.
    */

    //1 byte
    private final String OPCODE = "01";

    //1 byte - return value
    private final String[] exitEvtAndReturnReturnValue = new String[1];

    private final int size = 2;

    private final String displayName = "01 - event end and return";

    public ExitEvtAndReturn_evtEnd_01(String exitEvtAndReturnValue) {
        if(exitEvtAndReturnValue != null) {this.exitEvtAndReturnReturnValue[0] = exitEvtAndReturnValue.substring(2, 4);}
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + exitEvtAndReturnReturnValue[0];

        return newScdDataByteString;
    }
}
