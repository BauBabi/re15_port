/* FUN_80059c74 @ 0x80059c74  (Ghidra headless, raw MIPS overlay) */

void FUN_80059c74(uint param_1)

{
  short sVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  DAT_800e2a84 = 1;
  while( true ) {
    FUN_80031f94(1);
    SpuSetTransferMode(0);
    if (DAT_800d4c49 != -1) {
      SsVabClose((short)DAT_800d4c49);
      DAT_800d4c49 = -1;
    }
    iVar2 = FUN_80012fb8(*(undefined2 *)(&DAT_800a8118 + (param_1 & 0xff) * 2),&DAT_801faa00,1,
                         "              ARMS EDT");
    iVar2 = *(int *)(&DAT_801fa9f8 + iVar2);
    DAT_800d75a4 = &DAT_801faa00 + iVar2;
    iVar3 = 0x96;
    if (36000 < (*(int *)(&DAT_801faa0c + iVar2) + -0xa10 +
                 (uint)*(ushort *)(&DAT_801faa12 + iVar2) * -0x200 & 0xfffffff0)) break;
    do {
      FUN_80031f94(1);
      sVar1 = SsVabTransCompleted(0);
    } while (sVar1 == 0);
    sVar1 = SsVabOpenHeadSticky(DAT_800d75a4,1,0xb7a0);
    DAT_800d4c49 = (char)sVar1;
    if (DAT_800d4c49 == -1) {
      iVar4 = iVar4 + 1;
      FUN_800594c8(1,0);
      if (iVar4 == 8) {
        FUN_80031f94(1);
        return;
      }
    }
    else {
      iVar4 = 0;
      iVar2 = FUN_800132b0(*(undefined2 *)(&DAT_800a8140 + (param_1 & 0xff) * 2),0,&DAT_801bfa14,
                           (int)DAT_800d4c49,"              ARMS VBD");
      if (iVar2 != -1) {
        FUN_80029c94();
        FUN_80031f94(1);
        DAT_800e2a84 = 0;
        return;
      }
    }
  }
  do {
    FUN_80031f94(1);
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}


