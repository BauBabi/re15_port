void FUN_80015850(void)

{
  int *piVar1;
  int iVar2;
  
  DAT_800b5258 = DAT_800b5258 - DAT_800b5260;
  DAT_800b5238 = DAT_800b5238 - DAT_800b5224;
  iVar2 = rcos((uint)DAT_800b525c);
  DAT_800b5234 = (iVar2 * DAT_800b5258 >> 0xc) + *(int *)(DAT_800b5250 + 0x14);
  iVar2 = rsin((uint)DAT_800b525c);
  piVar1 = (int *)(DAT_800b5250 + 0x1c);
  DAT_800b525c = DAT_800b525c - DAT_800b525e & 0xfff;
  DAT_800b5250 = 0x40;
  DAT_800b523c = (iVar2 * DAT_800b5258 >> 0xc) + *piVar1;
  DAT_800b5254 = DAT_800b5254 + (short)((iRam00000054 - DAT_800b5254) / 0x3c);
  DAT_800b5256 = DAT_800b5256 + (short)((iRam0000005c - DAT_800b5256) / 0x3c);
  _DAT_800b5240 = (iRam00000054 - _DAT_800b5240) / 0x3c + _DAT_800b5240;
  _DAT_800b5248 = (iRam0000005c - _DAT_800b5248) / 0x3c + _DAT_800b5248;
  _DAT_800b5244 = (iRam00000058 - (_DAT_800b5244 + 400)) / 0x14 + _DAT_800b5244;
  FUN_80053ca4(&DAT_800b5230);
  return;
}
