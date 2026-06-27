void FUN_8001a8f8(short *param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = FUN_8001a6d4((int)*(short *)(DAT_800ac784 + 0x34),(int)*(short *)(DAT_800ac784 + 0x3c),
                       (int)*param_1,(int)param_1[4]);
  if (param_2 << 0x10 < 0) {
    param_2 = -((param_2 << 0x10) >> 0x10);
    uVar1 = uVar1 + 0x800 & 0xfff;
  }
  uVar2 = param_2 + (uVar1 - *(ushort *)(DAT_800ac784 + 0x6a)) & 0xfff;
  if ((int)uVar2 < (param_2 << 0x10) >> 0xf) {
    *(short *)(DAT_800ac784 + 0x6a) = (short)uVar1;
  }
  else {
    *(ushort *)(DAT_800ac784 + 0x6a) = *(ushort *)(DAT_800ac784 + 0x6a) - (short)param_2;
    if (uVar2 < 0x801) {
      *(short *)(DAT_800ac784 + 0x6a) = *(short *)(DAT_800ac784 + 0x6a) + (short)param_2 * 2;
    }
  }
  return;
}
