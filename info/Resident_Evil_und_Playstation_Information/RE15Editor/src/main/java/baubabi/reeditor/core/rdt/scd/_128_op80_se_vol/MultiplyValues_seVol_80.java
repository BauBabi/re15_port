package baubabi.reeditor.core.rdt.scd._128_op80_se_vol;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class MultiplyValues_seVol_80 {
    //2 Byte

    /*
        Multiply some values
    */

    //1 byte
    private final String OPCODE = "80"; /* 0x76 */

    //1 byte - the value to multiply
    private final String[] multiplyValuesValue = new String[1];

    private final int size = 2;

    private final String displayName = OPCODE + " - multiply values";

    public MultiplyValues_seVol_80(String multiplyValuesValue) {
        if(multiplyValuesValue != null) {
            this.multiplyValuesValue[0] = multiplyValuesValue.substring(2, 4);
        }
    }
    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + multiplyValuesValue[0];

        return newScdDataByteString;
    }
}
