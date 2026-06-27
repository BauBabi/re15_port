uint FUN_8001af20(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = param_1 >> 7 & 0xff;
  uVar2 = param_1 + uVar1 & 0xff;
  DAT_800ac774 = uVar2 | uVar1 << 8;
  return uVar2;
}
