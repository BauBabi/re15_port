byte * FUN_80013160(byte *param_1,uint *param_2)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = (uint)DAT_800c44b6;
  if (uVar1 != 0xffff) {
    DAT_800c44b6 = 0xffff;
    *param_2 = uVar1;
    return param_1 + 1;
  }
  if (0x58 < (*param_1 + 0xa0 & 0xff)) {
    *param_2 = (uint)*param_1;
    return param_1 + 1;
  }
  iVar2 = (*param_1 - 0xa0) * 2;
  DAT_800c44b6 = (ushort)(byte)(&DAT_800c44b9)[iVar2];
  *param_2 = (uint)(byte)(&DAT_800c44b8)[iVar2];
  return param_1;
}
