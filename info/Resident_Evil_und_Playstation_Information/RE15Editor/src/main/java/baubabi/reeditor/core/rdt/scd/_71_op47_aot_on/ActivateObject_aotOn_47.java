package baubabi.reeditor.core.rdt.scd._71_op47_aot_on;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ActivateObject_aotOn_47 {
    //2 Byte

    /*
        Activate object, present in the room objects list. For example, use a computer or open a desk.
    */

    //1 byte
    private final String OPCODE = "47"; /* 0x47 */

    //1 byte - index of room object list
    private final String[] activateObjectId = new String[1]; /* Index in list of room objects (doors, items, etc) */

    private final int size = 2;

    private final String displayName = OPCODE + " - activate object";

    public ActivateObject_aotOn_47(String activateObjectValue) {
        if(activateObjectValue != null) {
            this.activateObjectId[0] = activateObjectValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + activateObjectId[0];

        return newScdDataByteString;
    }
}
