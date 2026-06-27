package baubabi.reeditor.core.rdt.scd._46_op2E_work_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class SelectObjectForInstruction_workSet_2E {
    //3 Byte

    /*
        This instruction select current object (player, entity, room object) on which further instructions will apply.
    */

    //1 byte
    private final String OPCODE = "2E";

    //2 byte - The Parameter to select the Object
    /* component:index */
    /* 0x01:0x00: player entity */
    /* 0x03:NN: entity NN (enemy, etc) */
    /* 0x04:NN: room entity NN (ladder, door, etc) */
    private final String[] selectObjectForInstructionParam = new String[2];

    private final int size = 3;

    private final String displayName = "2E - select object for instruction (work set)";

    public SelectObjectForInstruction_workSet_2E(String selectObjectForInstructionValue) {
        if(selectObjectForInstructionValue != null) {
            this.selectObjectForInstructionParam[0] = selectObjectForInstructionValue.substring(2,4);
            this.selectObjectForInstructionParam[1] = selectObjectForInstructionValue.substring(4,6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + selectObjectForInstructionParam[0];
        newScdDataByteString = newScdDataByteString + selectObjectForInstructionParam[1];

        return newScdDataByteString;
    }
}
