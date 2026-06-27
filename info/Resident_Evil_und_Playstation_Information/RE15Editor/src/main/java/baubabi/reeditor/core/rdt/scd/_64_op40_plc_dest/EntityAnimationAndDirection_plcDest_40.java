package baubabi.reeditor.core.rdt.scd._64_op40_plc_dest;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class EntityAnimationAndDirection_plcDest_40 {
    //8 Byte

    /*
        Select animation pose and direction of current active entity.
    */

    //1 byte
    private final String OPCODE = "40"; /* 0x40 */

    //1 byte - the animation and direction dummy
    private final String[] entityAnimationAndDirectionDummy = new String[1];

    //2 byte - unknown value 1 - array of 2 with 1 byte each
    private final String[] entityAnimationAndDirectionUnknown1 = new String[2];

    //4 byte - unknown value 2 - array of 2 with 2 byte each
    private final String[] entityAnimationAndDirectionUnknown2 = new String[4];

    private final int size = 8;

    private final String displayName = "40 - entity animation and direction";

    public EntityAnimationAndDirection_plcDest_40(String entityAnimationAndDirectionValue) {
        if(entityAnimationAndDirectionValue != null) {
            this.entityAnimationAndDirectionDummy[0] = entityAnimationAndDirectionValue.substring(2,4);
            this.entityAnimationAndDirectionUnknown1[0] = entityAnimationAndDirectionValue.substring(4,6);
            this.entityAnimationAndDirectionUnknown1[1] = entityAnimationAndDirectionValue.substring(6,8);
            this.entityAnimationAndDirectionUnknown2[0] = entityAnimationAndDirectionValue.substring(8,10);
            this.entityAnimationAndDirectionUnknown2[1] = entityAnimationAndDirectionValue.substring(10,12);
            this.entityAnimationAndDirectionUnknown2[2] = entityAnimationAndDirectionValue.substring(12,14);
            this.entityAnimationAndDirectionUnknown2[3] = entityAnimationAndDirectionValue.substring(14,16);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + entityAnimationAndDirectionDummy[0];
        newScdDataByteString = newScdDataByteString + entityAnimationAndDirectionUnknown1[0];
        newScdDataByteString = newScdDataByteString + entityAnimationAndDirectionUnknown1[1];
        newScdDataByteString = newScdDataByteString + entityAnimationAndDirectionUnknown2[0];
        newScdDataByteString = newScdDataByteString + entityAnimationAndDirectionUnknown2[1];
        newScdDataByteString = newScdDataByteString + entityAnimationAndDirectionUnknown2[2];
        newScdDataByteString = newScdDataByteString + entityAnimationAndDirectionUnknown2[3];

        return newScdDataByteString;
    }
}
