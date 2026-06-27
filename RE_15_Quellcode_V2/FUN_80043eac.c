void FUN_80043eac(void)

{
  short sVar1;
  int *piVar2;
  
  piVar2 = *(int **)(DAT_800ac778 + 0xc);
  if (*piVar2 != -1) {
    DAT_800b25a0 = piVar2;
    if (-1 < DAT_800b21ee) {
      SsVabClose((short)DAT_800b21ee);
      DAT_800b21ee = -1;
    }
    if (*DAT_800b25a0 != 0) {
      do {
        sVar1 = SsVabOpenHeadSticky((uchar *)DAT_800b25a0,2,0x2f3d0);
        DAT_800b21ee = (char)sVar1;
      } while (DAT_800b21ee == -1);
      do {
        sVar1 = SsVabTransBody(*(uchar **)(DAT_800ac778 + 0x10),(short)DAT_800b21ee);
      } while (sVar1 == -1);
      do {
        FUN_80029ac8(1);
        sVar1 = SsVabTransCompleted(0);
      } while (sVar1 == 0);
    }
  }
  return;
}
