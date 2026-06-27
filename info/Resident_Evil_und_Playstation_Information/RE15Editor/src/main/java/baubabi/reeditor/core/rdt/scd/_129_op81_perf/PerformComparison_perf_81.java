package baubabi.reeditor.core.rdt.scd._129_op81_perf;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class PerformComparison_perf_81 {
    //3 Byte

    /*
        Perform a comparison
    */

    //1 byte
    private final String OPCODE = "81";

    //2 byte - the compare data
    private final String[] performComparisonData = new String[2];

    private final int size = 3;

    private final String displayName = OPCODE + " - perform comparison";

    public PerformComparison_perf_81(String performComparisonValue) {
        if(performComparisonValue != null) {
            this.performComparisonData[0] = performComparisonValue.substring(2, 4);
            this.performComparisonData[1] = performComparisonValue.substring(4, 6);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + performComparisonData[0];
        newScdDataByteString = newScdDataByteString + performComparisonData[1];

        return newScdDataByteString;
    }
}
