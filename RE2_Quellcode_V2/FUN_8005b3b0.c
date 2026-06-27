/* FUN_8005b3b0 @ 0x8005b3b0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8005b3b0(void)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  short sVar4;
  long lVar5;
  char *pcVar6;
  char *pcVar7;
  int iVar8;
  
  if ((DAT_800cfb74 & 0x2000) == 0) {
    if (DAT_800dbb70 == 1) {
      DAT_800dfadf = DAT_800dfadf + -1;
      if (DAT_800df340 == '\x18') {
        uVar2 = ((_DAT_800dcb9c & 0xffff) * 0x62) / 100;
        uVar3 = ((_DAT_800dcb9c >> 0x10) * 0x62) / 100;
        _DAT_800dcb9c = CONCAT22((short)uVar3,(short)uVar2);
        SsSetMVol((short)(uVar2 * 0x10000 >> 0x10),(short)(uVar3 * 0x10000 >> 0x10));
      }
      iVar8 = 3;
      if (DAT_800dfadf == '\x01') {
        pcVar6 = (char *)&DAT_800d75d8;
        pcVar7 = &DAT_800d75d2;
        do {
          pcVar6 = pcVar6 + -8;
          iVar8 = iVar8 + -1;
          if (*pcVar6 != '\0') {
            *pcVar6 = '\0';
            sVar4 = SsIsEos((short)*pcVar7,0);
            if (sVar4 != 0) {
              FUN_8007eaf8((int)*pcVar7);
            }
          }
          pcVar7 = pcVar7 + -8;
        } while (iVar8 != 0);
        if (DAT_800df340 == '\x18') {
          _DAT_800dcb9c = 0x7f007f;
          SsSetMVol(0x7f,0x7f);
        }
        DAT_800dbb70 = 2;
      }
    }
    else if (DAT_800dbb70 < 2) {
      if (DAT_800dbb70 == 0) {
        if ((5 < DAT_800d75c0) &&
           (bVar1 = DAT_800d75c0 == 6, DAT_800d75c0 = DAT_800d75c0 - 1, bVar1)) {
          DAT_800d75c0 = 0;
          sVar4 = SsIsEos((short)DAT_800d75c2,0);
          if (sVar4 != 0) {
            FUN_8007eaf8((int)DAT_800d75c2);
          }
        }
        if ((5 < DAT_800d75c8) &&
           (bVar1 = DAT_800d75c8 == 6, DAT_800d75c8 = DAT_800d75c8 - 1, bVar1)) {
          DAT_800d75c8 = 0;
          sVar4 = SsIsEos((short)DAT_800d75ca,0);
          if (sVar4 != 0) {
            FUN_8007eaf8((int)DAT_800d75ca);
          }
        }
        if ((5 < DAT_800d75d0) &&
           (bVar1 = DAT_800d75d0 == 6, DAT_800d75d0 = DAT_800d75d0 - 1, bVar1)) {
          DAT_800d75d0 = 0;
          sVar4 = SsIsEos((short)DAT_800d75d2,0);
          if (sVar4 != 0) {
            FUN_8007eaf8((int)DAT_800d75d2);
          }
        }
      }
    }
    else if (DAT_800dbb70 == 2) {
      FUN_8007f358(0);
      do {
        lVar5 = SpuClearReverbWorkArea(3);
      } while (lVar5 == -1);
      DAT_800dfadf = '\0';
      DAT_800dbb70 = 0;
    }
  }
  return;
}


