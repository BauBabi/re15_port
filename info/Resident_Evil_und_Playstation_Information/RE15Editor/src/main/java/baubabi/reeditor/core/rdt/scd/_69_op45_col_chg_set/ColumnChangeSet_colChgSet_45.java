package baubabi.reeditor.core.rdt.scd._69_op45_col_chg_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ColumnChangeSet_colChgSet_45 {
    //5 Byte

    /*
        Something, Fully unknown.
    */

    //1 byte
    private final String OPCODE = "45";

    //4 byte - unknown value
    private final String[] columnChangeSetUnknown = new String[4];

    private final int size = 5;

    private final String displayName = OPCODE + " - column change set";

    public ColumnChangeSet_colChgSet_45(String columnChangeSetValue) {
        if(columnChangeSetValue != null) {
            this.columnChangeSetUnknown[0] = columnChangeSetValue.substring(2,4);
            this.columnChangeSetUnknown[1] = columnChangeSetValue.substring(4,6);
            this.columnChangeSetUnknown[2] = columnChangeSetValue.substring(6,8);
            this.columnChangeSetUnknown[3] = columnChangeSetValue.substring(8,10);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + columnChangeSetUnknown[0];
        newScdDataByteString = newScdDataByteString + columnChangeSetUnknown[1];
        newScdDataByteString = newScdDataByteString + columnChangeSetUnknown[2];
        newScdDataByteString = newScdDataByteString + columnChangeSetUnknown[3];

        return newScdDataByteString;
    }
}
