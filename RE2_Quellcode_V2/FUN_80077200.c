/* FUN_80077200 @ 0x80077200  (Ghidra headless, raw MIPS overlay) */

void FUN_80077200(int param_1,VECTOR *param_2,byte *param_3)

{
  byte *pbVar1;
  int iVar2;
  VECTOR local_28;
  
  iVar2 = param_1 * 2;
  param_1 = param_1 * 6;
  pbVar1 = (byte *)(*(int *)(DAT_800ce324 + 0x2c) + DAT_800d4820 * 0x28);
  VectorNormal(param_2,&local_28);
  *(ushort *)(&DAT_8009db64 + param_1) = (short)local_28.vx * -(ushort)*pbVar1;
  *(ushort *)(&DAT_8009db66 + param_1) = (short)local_28.vy * -(ushort)*pbVar1;
  *(ushort *)(&DAT_8009db68 + param_1) = (short)local_28.vz * -(ushort)*pbVar1;
  *(ushort *)(&DAT_8009db84 + iVar2) = (ushort)*param_3 << 4;
  *(ushort *)(&DAT_8009db8a + iVar2) = (ushort)param_3[1] << 4;
  *(ushort *)(&DAT_8009db90 + iVar2) = (ushort)param_3[2] << 4;
  return;
}


