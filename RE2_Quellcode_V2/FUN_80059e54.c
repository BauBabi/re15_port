/* FUN_80059e54 @ 0x80059e54  (Ghidra headless, raw MIPS overlay) */

void FUN_80059e54(void)

{
  short sVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = 0;
  iVar3 = 0;
  DAT_800e2a84 = 1;
  do {
    while( true ) {
      FUN_80031f94(1);
      SpuSetTransferMode(0);
      FUN_800597a4();
      if (DAT_800d4c4a != -1) {
        SsVabClose((short)DAT_800d4c4a);
        DAT_800d4c4a = -1;
      }
      if (DAT_800d4c4b != -1) {
        SsVabClose((short)DAT_800d4c4b);
        DAT_800d4c4b = -1;
      }
      if (*(int *)(DAT_800ce324 + 0xc) == 0) {
        DAT_800eae40 = 0;
        return;
      }
      DAT_800dbb80 = *(undefined4 *)(DAT_800ce324 + 8);
      DAT_800c4410 = *(char *)(DAT_800ce324 + 6) << 2;
      DAT_800d75a8 = *(uchar **)(DAT_800ce324 + 0xc);
      DAT_800eae40 = *(int *)(*(int *)(DAT_800ce324 + 0xc) + 0xc) + -0xa20 +
                     (uint)*(ushort *)(*(int *)(DAT_800ce324 + 0xc) + 0x12) * -0x200 & 0xfffffff0;
      DAT_800dbb8c = DAT_800dbb80;
      do {
        FUN_80031f94(1);
        sVar1 = SsVabTransCompleted(0);
      } while (sVar1 == 0);
      sVar1 = SsVabOpenHeadSticky(DAT_800d75a8,2,0x14440);
      DAT_800d4c4a = (char)sVar1;
      iVar2 = iVar2 + 1;
      if (DAT_800d4c4a != -1) break;
      FUN_800594c8(2,0);
      if (iVar2 == 8) goto LAB_8005a028;
    }
    iVar2 = 0;
    sVar1 = SsVabTransBody(*(uchar **)(DAT_800ce324 + 0x10),(short)DAT_800d4c4a);
    iVar3 = iVar3 + 1;
    if (sVar1 != -1) {
      DAT_800a7fac = 0;
      while (sVar1 = SsVabTransCompleted(0), sVar1 == 0) {
        FUN_8005cf70();
        FUN_80031f94(1);
      }
      FUN_80031f94(1);
      DAT_800e2a84 = 0;
      return;
    }
    FUN_800594c8((int)DAT_800d4c4a,0x14440);
  } while (iVar3 != 8);
LAB_8005a028:
  FUN_80031f94(1);
  return;
}


