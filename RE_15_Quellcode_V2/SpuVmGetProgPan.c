uint SpuVmGetProgPan(short param_1,short param_2)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = SpuVmVSetUp((int)param_1,(int)param_2);
  uVar2 = 0xffffffff;
  if (iVar1 == 0) {
    uVar2 = (uint)*(byte *)(param_2 * 0x10 + DAT_800b2b28 + 4);
  }
  return uVar2;
}
