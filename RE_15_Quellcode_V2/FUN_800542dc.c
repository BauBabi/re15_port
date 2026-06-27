void FUN_800542dc(int param_1,VECTOR *param_2,byte *param_3)

{
  int iVar1;
  VECTOR local_28;
  
  VectorNormal(param_2,&local_28);
  iVar1 = param_1 * 2;
  param_1 = param_1 * 6;
  *(ushort *)(&DAT_80076d14 + param_1) =
       -(ushort)*(byte *)(DAT_800b0fe4 * 0x28 + *(int *)(DAT_800ac778 + 0x2c)) * (short)local_28.vx;
  *(ushort *)(&DAT_80076d16 + param_1) =
       -(ushort)*(byte *)(DAT_800b0fe4 * 0x28 + *(int *)(DAT_800ac778 + 0x2c)) * (short)local_28.vy;
  *(ushort *)(&DAT_80076d18 + param_1) =
       -(ushort)*(byte *)(DAT_800b0fe4 * 0x28 + *(int *)(DAT_800ac778 + 0x2c)) * (short)local_28.vz;
  *(ushort *)(&DAT_80076d34 + iVar1) = (ushort)*param_3 << 4;
  *(ushort *)(&DAT_80076d3a + iVar1) = (ushort)param_3[1] << 4;
  *(ushort *)(&DAT_80076d40 + iVar1) = (ushort)param_3[2] << 4;
  return;
}
