/* FUN_8005a714 @ 0x8005a714  (Ghidra headless, raw MIPS overlay) */

void FUN_8005a714(void)

{
  bool bVar1;
  short sVar2;
  int iVar3;
  
  if ((DAT_800cfb74 & 0x2000) == 0) {
    DAT_800dfd64 = &DAT_800d46d0 + DAT_800d481e * 2 + (uint)(byte)(&DAT_800a80b0)[DAT_800d481c] * 2;
    if ((*DAT_800dfd64 & 0x3f) == (DAT_800d7856 & 0x3f)) {
      if ((DAT_800dfd64[1] & 0x3f) != (DAT_800d7857 & 0x3f)) {
        if ((DAT_800d75c8 == '\x01') && (sVar2 = SsIsEos((short)DAT_800d75ca,0), sVar2 != 0)) {
          FUN_8007a120((int)DAT_800d75ca,0x7f,0x5a);
          DAT_800d75c8 = '2';
        }
        if ((DAT_800d75d0 == '\x01') && (sVar2 = SsIsEos((short)DAT_800d75d2,0), sVar2 != 0)) {
          FUN_8007a120((int)DAT_800d75d2,0x7f,0x5a);
          DAT_800d75d0 = '2';
        }
      }
      iVar3 = 0x59;
      do {
        FUN_80031f94(1);
        bVar1 = iVar3 != 0;
        iVar3 = iVar3 + -1;
      } while (bVar1);
    }
    else {
      FUN_8005b2e4(0x5a,0x16);
    }
  }
  return;
}


