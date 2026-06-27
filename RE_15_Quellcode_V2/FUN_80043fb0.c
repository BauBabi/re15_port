void FUN_80043fb0(void)

{
  short sVar1;
  int *piVar2;
  
  piVar2 = *(int **)(DAT_800ac778 + 0x18);
  if (*piVar2 != -1) {
    DAT_800b25a4 = piVar2;
    if (-1 < DAT_800b21ef) {
      SsVabClose((short)DAT_800b21ef);
      DAT_800b21ef = -1;
    }
    if (*DAT_800b25a4 != 0) {
      do {
        sVar1 = SsVabOpenHeadSticky((uchar *)DAT_800b25a4,3,0xffd0);
        DAT_800b21ef = (char)sVar1;
        FUN_80029ac8(1);
      } while (DAT_800b21ef == -1);
      do {
        FUN_80029ac8(1);
        sVar1 = SsVabTransBody(*(uchar **)(DAT_800ac778 + 0x1c),(short)DAT_800b21ef);
      } while (sVar1 == -1);
      do {
        FUN_80029ac8(1);
        sVar1 = SsVabTransCompleted(0);
      } while (sVar1 == 0);
    }
  }
  return;
}
