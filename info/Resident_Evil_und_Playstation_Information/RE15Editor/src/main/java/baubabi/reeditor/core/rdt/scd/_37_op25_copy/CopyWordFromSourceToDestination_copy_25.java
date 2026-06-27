package baubabi.reeditor.core.rdt.scd._37_op25_copy;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class CopyWordFromSourceToDestination_copy_25 {
    //3 Byte

    /*
        Copy a word from source variable to destination variable
    */

    //1 byte
    private final String OPCODE = "25"; /* 0x25 */

    //1 byte - Index of destination variable
    private final String[] copyWordFromSourceToDestinationDestinationIndex = new String[1]; /* Index of destination variable */

    //1 byte - Index os source variable
    private final String[] copyWordFromSourceToDestinationSourceIndex = new String[1]; /* Index of source variable */

    private final int size = 3;

    private final String displayName = "25 - copy word from src to dst";

    public CopyWordFromSourceToDestination_copy_25(String copyWordFromSourceToDestinationValue) {
        if(copyWordFromSourceToDestinationValue != null) {
            this.copyWordFromSourceToDestinationDestinationIndex[0] = copyWordFromSourceToDestinationValue.substring(2,4);
            this.copyWordFromSourceToDestinationSourceIndex[0] = copyWordFromSourceToDestinationValue.substring(4,6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + copyWordFromSourceToDestinationDestinationIndex[0];
        newScdDataByteString = newScdDataByteString + copyWordFromSourceToDestinationSourceIndex[0];

        return newScdDataByteString;
    }
}
