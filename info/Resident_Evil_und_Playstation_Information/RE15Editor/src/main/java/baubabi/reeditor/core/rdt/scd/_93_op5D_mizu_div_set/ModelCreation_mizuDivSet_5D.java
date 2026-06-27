package baubabi.reeditor.core.rdt.scd._93_op5D_mizu_div_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class ModelCreation_mizuDivSet_5D {
    //2 Byte

    /*
        Seems related to 3D model creation
    */

    //1 byte
    private final String OPCODE = "5D"; /* 0x5d */

    //1 byte - unknown value
    private final String[] modelCreationUnknown = new String[1];

    private final int size = 2;

    private final String displayName = OPCODE + " - model creation";

    public ModelCreation_mizuDivSet_5D(String modelCreationValue) {
        if(modelCreationValue != null) {
            this.modelCreationUnknown[0] = modelCreationValue.substring(2, 4);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + modelCreationUnknown[0];

        return newScdDataByteString;
    }
}
