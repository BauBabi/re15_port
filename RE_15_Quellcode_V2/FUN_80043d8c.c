void FUN_80043d8c(uint param_1,uchar *param_2)

{
  short sVar1;
  int iVar2;
  
  if (-1 < DAT_800b21ed) {
    SsVabClose((short)DAT_800b21ed);
    DAT_800b21ed = -1;
  }
  do {
    iVar2 = FUN_80013b60(*(undefined2 *)(&DAT_8007492c + (param_1 & 0xff) * 2),&DAT_801fcd00,0);
    DAT_800b259c = &DAT_801fcd00 + *(int *)(&DAT_801fccf8 + iVar2);
    sVar1 = SsVabOpenHeadSticky(DAT_800b259c,1,0x7330);
    DAT_800b21ed = (char)sVar1;
  } while (DAT_800b21ed == -1);
  do {
    FUN_80013b60(*(undefined2 *)(&DAT_80074958 + (param_1 & 0xff) * 2),param_2,0);
    sVar1 = SsVabTransBody(param_2,(short)DAT_800b21ed);
  } while (sVar1 == -1);
  SsVabTransCompleted(1);
  return;
}
