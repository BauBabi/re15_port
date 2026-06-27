void FUN_8002d3d8(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = ((uint)*(ushort *)(param_1 + 0x34) - (uint)(ushort)DAT_800aca88) * 0x10000;
  iVar1 = iVar2 >> 0x10;
  if (iVar1 < 0) {
    iVar1 = -iVar1;
  }
  if (iVar1 < 900) {
    DAT_800b527c = (short)(*(short *)(param_1 + 0x3c) - (short)DAT_800aca90) >> 0xf | 1;
    DAT_800b526c = 0;
  }
  else {
    DAT_800b526c = (ushort)(iVar2 >> 0x1f) | 1;
    DAT_800b527c = 0;
  }
  DAT_800b282c = *(undefined2 *)(param_1 + 0x68);
  DAT_800b2838 = *(undefined2 *)(param_1 + 0x6c);
  return;
}
