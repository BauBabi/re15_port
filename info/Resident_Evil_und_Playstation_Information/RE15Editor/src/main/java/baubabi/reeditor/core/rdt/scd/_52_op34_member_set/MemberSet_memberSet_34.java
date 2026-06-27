package baubabi.reeditor.core.rdt.scd._52_op34_member_set;

import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
@NoArgsConstructor
public class MemberSet_memberSet_34 {
    //4 Byte

    /*
        Set value in the structure of an entity (EM_SET).

        //short unknown0[2];	// 0x00
        //char unknown1[5];
        //char dummy0;
        //char unknown2[2];
        //long dummy1;
        //long unknown3;		// 0x10
        //long dummy2[3+4+2];
        //long unknown4[3];	// 0x38
        //long dummy3[3+4+4+1];
        //short unknown5[3];	// 0x74
        //long dummy4[23];
        //char unknown6;		// 0x106
        //char dummy5[77];
        //short unknown7;		// 0x154
        //char dummy6[108];
        //short unknown8[3];	// 0x1c2
        //long dummy7;
        //short unknown9;		// 0x1cc
        //long dummy8;
        //short unknown10[4];	// 0x1d4
    */

    //1 byte
    private final String OPCODE = "34";

    //3 byte - Unknown byte data
    private final String[] memberSetByteData = new String[3];

    private final int size = 4;

    private final String displayName = "34 - member set";

    public MemberSet_memberSet_34(String memberSetValue) {
        if(memberSetValue != null) {
            this.memberSetByteData[0] = memberSetValue.substring(2,4);
            this.memberSetByteData[1] = memberSetValue.substring(4,6);
            this.memberSetByteData[2] = memberSetValue.substring(6,8);
        }
    }

    public String prepareNewScdDataByteForSaving(String newScdDataByteString) {
        newScdDataByteString = newScdDataByteString + OPCODE;
        newScdDataByteString = newScdDataByteString + memberSetByteData[0];
        newScdDataByteString = newScdDataByteString + memberSetByteData[1];
        newScdDataByteString = newScdDataByteString + memberSetByteData[2];

        return newScdDataByteString;
    }
}
