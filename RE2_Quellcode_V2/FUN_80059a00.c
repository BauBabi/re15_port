/* FUN_80059a00 @ 0x80059a00  (Ghidra headless, raw MIPS overlay) */

void FUN_80059a00(uint param_1,char param_2)

{
  short sVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = 0;
  iVar4 = 0;
  DAT_800e2a84 = 1;
  do {
    while( true ) {
      FUN_80031f94(1);
      SpuSetTransferMode(0);
      if (DAT_800d4c4c != -1) {
        SsVabClose((short)DAT_800d4c4c);
        DAT_800d4c4c = -1;
      }
      DAT_800dbb88 = &DAT_801f9d10;
      iVar2 = FUN_80012fb8(*(undefined2 *)(&DAT_800a80b8 + (param_1 & 0xff) * 2),&DAT_801f9d10,1,
                           "              CORE EDH");
      DAT_800d75b0 = &DAT_801f9d10 + *(int *)(&DAT_801f9d08 + iVar2);
      do {
        FUN_80031f94(1);
        sVar1 = SsVabTransCompleted(0);
      } while (sVar1 == 0);
      sVar1 = SsVabOpenHeadSticky(DAT_800d75b0,4,0x1020);
      DAT_800d4c4c = (char)sVar1;
      if (DAT_800d4c4c == -1) break;
      iVar3 = 0;
      if (param_2 == '\x04') {
        FUN_80012fb8(*(undefined2 *)(&DAT_800a80e8 + (param_1 & 0xff) * 2),&DAT_80198000,0,
                     "              CORE VBD");
        sVar1 = SsVabTransBody((uchar *)&DAT_80198000,(short)DAT_800d4c4c);
        iVar4 = iVar4 + 1;
        if (sVar1 != -1) {
          DAT_800a7fac = 0;
          while (sVar1 = SsVabTransCompleted(0), sVar1 == 0) {
            FUN_8005cf70();
            FUN_80031f94(1);
          }
LAB_80059c3c:
          FUN_80031f94(1);
          DAT_800e2a84 = 0;
          return;
        }
        FUN_800594c8((int)DAT_800d4c4c,0x1020);
        if (iVar4 == 8) goto LAB_80059bac;
      }
      else {
        iVar2 = FUN_800132b0(*(undefined2 *)(&DAT_800a80e8 + (param_1 & 0xff) * 2),0,&DAT_801bfa14,
                             (int)DAT_800d4c4c,"              CORE VBD");
        if (iVar2 != -1) {
          FUN_80029c94();
          goto LAB_80059c3c;
        }
      }
    }
    iVar3 = iVar3 + 1;
    FUN_800594c8(4,0);
  } while (iVar3 != 8);
LAB_80059bac:
  FUN_80031f94(1);
  return;
}


