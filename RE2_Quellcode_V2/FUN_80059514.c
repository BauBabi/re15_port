/* FUN_80059514 @ 0x80059514  (Ghidra headless, raw MIPS overlay) */

void FUN_80059514(void)

{
  short sVar1;
  char *pcVar2;
  int iVar3;
  
  pcVar2 = &DAT_800d75c2;
  iVar3 = 0;
  DAT_800eae40 = 0;
  DAT_800eade4 = 0;
  DAT_800c4410 = 0;
  do {
    if ((&DAT_800d75c0)[iVar3] != '\0') {
      sVar1 = SsIsEos((short)*pcVar2,0);
      if (sVar1 != 0) {
        FUN_8007eaf8((int)*pcVar2);
      }
      (&DAT_800d75c0)[iVar3] = 0;
    }
    if (-1 < *pcVar2) {
      SsSeqClose((short)*pcVar2);
      (&DAT_800d75c2)[iVar3] = 0xff;
    }
    pcVar2 = pcVar2 + 8;
    iVar3 = iVar3 + 8;
  } while (pcVar2 < &DAT_800d75da);
  FUN_8007f358(0);
  pcVar2 = &DAT_800d4c48;
  do {
    if (*pcVar2 != -1) {
      SsVabClose((short)*pcVar2);
      *pcVar2 = -1;
    }
    pcVar2 = pcVar2 + 1;
  } while (pcVar2 < "");
  return;
}


