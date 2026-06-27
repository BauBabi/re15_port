void FUN_80017048(uint param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = (&DAT_800b23f4)[param_1 & 0xff];
  iVar1 = param_2 * 0x38 + DAT_800b8554;
  *(int *)(iVar2 + 0x10) = iVar1;
  *(int *)(iVar2 + 0x18) = iVar1 + 0x1c;
  *(undefined4 *)(iVar2 + 0x70) = 0x808080;
  DAT_800b8550 = FUN_80025940(iVar2 + 0xc,DAT_800b8550);
  DAT_800b8550 = FUN_80025a98(iVar2 + 0xc);
  return;
}
