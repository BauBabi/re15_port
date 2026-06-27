/* FUN_8001653c @ 0x8001653c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001653c(undefined4 *param_1,undefined4 param_2,undefined4 param_3,uint param_4,
                 undefined4 param_5)

{
  char *pcVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined *puVar4;
  undefined *puVar5;
  uint *puVar6;
  int iVar7;
  int iVar8;
  
  puVar2 = (undefined4 *)&UNK_800ce60c;
  iVar7 = 5;
  if (DAT_800ce61a != '\0') {
    puVar3 = puVar2;
    iVar8 = 4;
    do {
      iVar7 = iVar8;
      puVar2 = puVar3 + 7;
      if (iVar7 == 0) {
        return;
      }
      pcVar1 = (char *)((int)puVar3 + 0x2a);
      puVar3 = puVar2;
      iVar8 = iVar7 + -1;
    } while (*pcVar1 != '\0');
  }
  if (iVar7 != 0) {
    *(undefined1 *)((int)puVar2 + 0xe) = 7;
    puVar4 = &UNK_800ce698;
    iVar7 = 0x29;
    if (DAT_800ce6a6 != '\0') {
      puVar5 = puVar4;
      iVar8 = 0x28;
      do {
        iVar7 = iVar8;
        puVar4 = puVar5 + 0x68;
        if (iVar7 == 0) goto LAB_800165c4;
        pcVar1 = puVar5 + 0x76;
        puVar5 = puVar4;
        iVar8 = iVar7 + -1;
      } while (*pcVar1 != '\0');
    }
    iVar8 = 9;
    if (iVar7 == 0) {
LAB_800165c4:
      *(undefined1 *)((int)puVar2 + 0xe) = 0;
      *param_1 = 0;
    }
    else {
      puVar6 = (uint *)(puVar4 + 0x44);
      *param_1 = puVar2;
      puVar2[6] = puVar4;
      *puVar2 = param_2;
      puVar2[1] = param_3;
      puVar2[5] = param_5;
      do {
        puVar5 = puVar4;
        *(undefined1 *)((int)puVar6 + -0x36) = 2;
        if (param_4 != 0) {
          puVar6[-10] = puVar6[-10] & 0xff000000 | param_4;
          *puVar6 = *puVar6 & 0xff000000 | param_4;
        }
        iVar8 = iVar8 + -1;
        puVar6 = puVar6 + 0x1a;
        puVar4 = puVar5 + 0x68;
      } while (iVar8 != 0);
      puVar5[-0x31c] = 0x62;
      puVar5[-0x31b] = 0xe1;
      puVar5[-0x314] = 0x6b;
      puVar5[-0x313] = 0xe1;
      puVar5[-0x30c] = 0x62;
      puVar5[-0x30b] = 0xeb;
      puVar5[-0x304] = 0x6b;
      puVar5[-0x303] = 0xeb;
      puVar5[-0x2f4] = 0x62;
      puVar5[-0x2f3] = 0xe1;
      puVar5[-0x2ec] = 0x6b;
      puVar5[-0x2eb] = 0xe1;
      puVar5[-0x2e4] = 0x62;
      puVar5[-0x2e3] = 0xeb;
      puVar5[-0x2dc] = 0x6b;
      puVar5[-0x2db] = 0xeb;
      puVar5[-0x2b4] = 0x6b;
      puVar5[-0x2b3] = 0xe1;
      puVar5[-0x2ac] = 0x74;
      puVar5[-0x2ab] = 0xe1;
      puVar5[-0x2a4] = 0x6b;
      puVar5[-0x2a3] = 0xeb;
      puVar5[-0x29c] = 0x74;
      puVar5[-0x29b] = 0xeb;
      puVar5[-0x28c] = 0x6b;
      puVar5[-0x28b] = 0xe1;
      puVar5[-0x284] = 0x74;
      puVar5[-0x283] = 0xe1;
      puVar5[-0x27c] = 0x6b;
      puVar5[-0x27b] = 0xeb;
      puVar5[-0x274] = 0x74;
      puVar5[-0x273] = 0xeb;
      puVar5[-0x24c] = 0x74;
      puVar5[-0x24b] = 0xe1;
      puVar5[-0x244] = 0x7e;
      puVar5[-0x243] = 0xe1;
      puVar5[-0x23c] = 0x74;
      puVar5[-0x23b] = 0xeb;
      puVar5[-0x234] = 0x7e;
      puVar5[-0x233] = 0xeb;
      puVar5[-0x224] = 0x74;
      puVar5[-0x223] = 0xe1;
      puVar5[-0x21c] = 0x7e;
      puVar5[-0x21b] = 0xe1;
      puVar5[-0x214] = 0x74;
      puVar5[-0x213] = 0xeb;
      puVar5[-0x20c] = 0x7e;
      puVar5[-0x20b] = 0xeb;
      puVar5[-0x1e4] = 0x62;
      puVar5[-0x1e3] = 0xeb;
      puVar5[-0x1dc] = 0x6b;
      puVar5[-0x1db] = 0xeb;
      puVar5[-0x1d4] = 0x62;
      puVar5[-0x1d3] = 0xf5;
      puVar5[-0x1cc] = 0x6b;
      puVar5[-0x1cb] = 0xf5;
      puVar5[-0x1bc] = 0x62;
      puVar5[-0x1bb] = 0xeb;
      puVar5[-0x1b4] = 0x6b;
      puVar5[-0x1b3] = 0xeb;
      puVar5[-0x1ac] = 0x62;
      puVar5[-0x1ab] = 0xf5;
      puVar5[-0x1a4] = 0x6b;
      puVar5[-0x1a3] = 0xf5;
      puVar5[-0x17c] = 0x6b;
      puVar5[-0x17b] = 0xeb;
      puVar5[-0x174] = 0x74;
      puVar5[-0x173] = 0xeb;
      puVar5[-0x16c] = 0x6b;
      puVar5[-0x16b] = 0xf5;
      puVar5[-0x164] = 0x74;
      puVar5[-0x163] = 0xf5;
      puVar5[-0x154] = 0x6b;
      puVar5[-0x153] = 0xeb;
      puVar5[-0x14c] = 0x74;
      puVar5[-0x14b] = 0xeb;
      puVar5[-0x144] = 0x6b;
      puVar5[-0x143] = 0xf5;
      puVar5[-0x13c] = 0x74;
      puVar5[-0x13b] = 0xf5;
      puVar5[-0x114] = 0x74;
      puVar5[-0x113] = 0xeb;
      puVar5[-0x10c] = 0x7e;
      puVar5[-0x10b] = 0xeb;
      puVar5[-0x104] = 0x74;
      puVar5[-0x103] = 0xf5;
      puVar5[-0xfc] = 0x7e;
      puVar5[-0xfb] = 0xf5;
      puVar5[-0xec] = 0x74;
      puVar5[-0xeb] = 0xeb;
      puVar5[-0xe4] = 0x7e;
      puVar5[-0xe3] = 0xeb;
      puVar5[-0xdc] = 0x74;
      puVar5[-0xdb] = 0xf5;
      puVar5[-0xd4] = 0x7e;
      puVar5[-0xd3] = 0xf5;
      puVar5[-0xac] = 0x62;
      puVar5[-0xab] = 0xf5;
      puVar5[-0xa4] = 0x6b;
      puVar5[-0xa3] = 0xf5;
      puVar5[-0x9c] = 0x62;
      puVar5[-0x9b] = 0xff;
      puVar5[-0x94] = 0x6b;
      puVar5[-0x93] = 0xff;
      puVar5[-0x84] = 0x62;
      puVar5[-0x83] = 0xf5;
      puVar5[-0x7c] = 0x6b;
      puVar5[-0x7b] = 0xf5;
      puVar5[-0x74] = 0x62;
      puVar5[-0x73] = 0xff;
      puVar5[-0x6c] = 0x6b;
      puVar5[-0x6b] = 0xff;
      puVar5[-0x44] = 0x6b;
      puVar5[-0x43] = 0xf5;
      puVar5[-0x3c] = 0x74;
      puVar5[-0x3b] = 0xf5;
      puVar5[-0x34] = 0x6b;
      puVar5[-0x33] = 0xff;
      puVar5[-0x2c] = 0x74;
      puVar5[-0x2b] = 0xff;
      puVar5[-0x1c] = 0x6b;
      puVar5[-0x1b] = 0xf5;
      puVar5[-0x14] = 0x74;
      puVar5[-0x13] = 0xf5;
      puVar5[-0xc] = 0x6b;
      puVar5[-0xb] = 0xff;
      puVar5[-4] = 0x74;
      puVar5[-3] = 0xff;
      puVar5[0x24] = 0x74;
      puVar5[0x25] = 0xf5;
      puVar5[0x2c] = 0x7e;
      puVar5[0x2d] = 0xf5;
      puVar5[0x34] = 0x74;
      puVar5[0x35] = 0xff;
      puVar5[0x3c] = 0x7e;
      puVar5[0x3d] = 0xff;
      puVar5[0x4c] = 0x74;
      puVar5[0x4d] = 0xf5;
      puVar5[0x54] = 0x7e;
      puVar5[0x55] = 0xf5;
      puVar5[0x5c] = 0x74;
      puVar5[0x5d] = 0xff;
      puVar5[100] = 0x7e;
      puVar5[0x65] = 0xff;
    }
  }
  return;
}


