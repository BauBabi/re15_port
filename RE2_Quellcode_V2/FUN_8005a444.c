/* FUN_8005a444 @ 0x8005a444  (Ghidra headless, raw MIPS overlay) */

void FUN_8005a444(void)

{
  byte bVar1;
  byte bVar2;
  bool bVar3;
  char cVar4;
  int iVar5;
  
  if ((DAT_800cfb74 & 0x2000) == 0) {
    DAT_800dfd64 = &DAT_800d46d0 + DAT_800d481e * 2 + (uint)(byte)(&DAT_800a80b0)[DAT_800d481c] * 2;
    bVar1 = *DAT_800dfd64;
    bVar2 = DAT_800dfd64[1];
    if (DAT_800dbb70 != '\0') {
      do {
        FUN_80031f94(1);
      } while (DAT_800dbb70 != '\0');
    }
    if ((bVar1 & 0x3f) == (DAT_800d7856 & 0x3f)) {
      bVar3 = false;
      if (bVar1 >> 6 == 0) {
        if (DAT_800d75c0 == '\x02') {
          if (-1 < DAT_800d75c2) {
            SsSeqPlay((short)DAT_800d75c2,'\0',1);
            iVar5 = (int)((((uint)DAT_800d75c4 * (uint)DAT_800eae3c) / 100) * 0x10000) >> 0x10;
            FUN_8007f018((int)DAT_800d75c2,iVar5,iVar5);
          }
          DAT_800d75c1 = 0;
        }
      }
      else if ((DAT_800d75c0 != '\0') && (DAT_800d75c0 != -1)) {
        FUN_8007a120((int)DAT_800d75c2,0x7f,0x5a);
        DAT_800d75c0 = '\x0f';
      }
    }
    else {
      bVar3 = true;
      cVar4 = FUN_8005a97c();
      if (cVar4 == '\0') {
        DAT_800d75c1 = bVar1 >> 6;
      }
      else {
        DAT_800d75c1 = 0xff;
      }
    }
    if (((bVar2 & 0x3f) != (DAT_800d7857 & 0x3f)) || (bVar3)) {
      cVar4 = FUN_8005ae50();
      if (cVar4 == '\0') {
        DAT_800d75d1 = bVar2 >> 7;
        DAT_800d75c9 = bVar2 >> 6 & 1;
      }
      else {
        DAT_800d75c9 = 0xff;
        DAT_800d75d1 = 0xff;
      }
    }
    else {
      if (((bVar2 >> 6 & 1) != 0) && (DAT_800d75c8 != '\0')) {
        FUN_8007a120((int)DAT_800d75ca,0x7f,0x5a);
      }
      if (((char)bVar2 < '\0') && (DAT_800d75d0 != '\0')) {
        FUN_8007a120((int)DAT_800d75d2,0x7f,0x5a);
      }
    }
    DAT_800d7856 = *DAT_800dfd64;
    DAT_800d7857 = DAT_800dfd64[1];
  }
  return;
}


