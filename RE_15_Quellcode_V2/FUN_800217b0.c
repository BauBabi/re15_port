void FUN_800217b0(uint param_1,undefined2 param_2,uint param_3,undefined1 param_4)

{
  int iVar1;
  
  iVar1 = (param_1 & 0xff) * 0x44;
  (&DAT_800b545a)[(param_1 & 0xff) * 0x22] = param_2;
  (&DAT_800b545c)[iVar1] = (char)(param_1 >> 8);
  (&DAT_800b5460)[iVar1] = param_4;
  if ((param_3 & 4) == 0) {
    (&DAT_800b545d)[iVar1] = 0;
  }
  else {
    (&DAT_800b545d)[iVar1] = 0xff;
  }
  if ((param_3 & 2) == 0) {
    (&DAT_800b545e)[iVar1] = 0;
  }
  else {
    (&DAT_800b545e)[iVar1] = 0xff;
  }
  if ((param_3 & 1) == 0) {
    (&DAT_800b545f)[iVar1] = 0;
  }
  else {
    (&DAT_800b545f)[iVar1] = 0xff;
  }
  SetDrawMode((DR_MODE *)(&UNK_800b5484 + iVar1),1,0,(uint)(byte)(&DAT_800b545c)[iVar1] << 5,
              (RECT *)0x0);
  SetDrawMode((DR_MODE *)(&UNK_800b5490 + iVar1),1,0,(uint)(byte)(&DAT_800b545c)[iVar1] << 5,
              (RECT *)0x0);
  return;
}
