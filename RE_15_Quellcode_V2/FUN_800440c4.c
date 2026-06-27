void FUN_800440c4(uint param_1)

{
  short sVar1;
  int iVar2;
  
  if (-1 < DAT_800b21f0) {
    SsVabClose((short)DAT_800b21f0);
    DAT_800b21f0 = -1;
  }
  do {
    iVar2 = FUN_80013b60(*(undefined2 *)(&DAT_80073a88 + (param_1 & 0xff) * 2),&DAT_801fbd00,1,
                         "CORE EDH");
    DAT_800b25a8 = &DAT_801fbd00 + *(int *)(&DAT_801fbcf8 + iVar2);
    sVar1 = SsVabOpenHeadSticky(DAT_800b25a8,4,0x38840);
    DAT_800b21f0 = (char)sVar1;
  } while (DAT_800b21f0 == -1);
  do {
    FUN_80013b60(*(undefined2 *)(&DAT_80073ab0 + (param_1 & 0xff) * 2),DAT_800ac77c,1,"CORE VBD");
    sVar1 = SsVabTransBody(DAT_800ac77c,(short)DAT_800b21f0);
  } while (sVar1 == -1);
  do {
    FUN_80029ac8(1);
    sVar1 = SsVabTransCompleted(0);
  } while (sVar1 == 0);
  return;
}
