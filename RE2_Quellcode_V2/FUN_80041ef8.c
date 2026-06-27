/* FUN_80041ef8 @ 0x80041ef8  (Ghidra headless, raw MIPS overlay) */

bool FUN_80041ef8(long *param_1,short param_2,int *param_3,short *param_4)

{
  bool bVar1;
  SVECTOR local_c0;
  SVECTOR local_b8;
  SVECTOR local_b0;
  uint local_a8;
  short local_a4;
  int local_a0;
  short local_9c;
  int local_98;
  short local_94;
  MATRIX local_90;
  MATRIX local_70;
  MATRIX MStack_50;
  SVECTOR local_30 [2];
  
  local_90.t[0] = (long)*param_4;
  local_90.t[1] = 0;
  local_90.m[0][0] = 0x1000;
  local_90.m[0][1] = 0;
  local_90.m[0][2] = 0;
  local_90.m[1][0] = 0;
  local_90.m[1][1] = 0x1000;
  local_90.m[1][2] = 0;
  local_90.m[2][0] = 0;
  local_90.m[2][1] = 0;
  local_90.m[2][2] = 0x1000;
  local_90._18_2_ = 0;
  local_30[0].vx = 0;
  local_30[0].vz = 0;
  local_90.t[2] = param_4[3] * -4 - (int)param_4[1];
  local_30[0].vy = param_2;
  RotMatrix(local_30,&MStack_50);
  MStack_50.t[0] = *param_1;
  MStack_50.t[1] = 0;
  MStack_50.t[2] = param_1[2];
  FUN_8002ce94(&MStack_50,&local_90,&local_70);
  local_a8 = local_70.t[0] >> 2 & 0xffff;
  local_a4 = (short)(local_70.t[2] >> 2);
  local_c0._0_4_ = SEXT24(param_4[2]);
  local_c0.vz = 0;
  local_c0.pad = 0;
  ApplyMatrixSV(&MStack_50,&local_c0,&local_b8);
  local_c0.vx = 0;
  local_c0.vy = 0;
  local_c0._4_4_ = (int)param_4[3] << 1;
  ApplyMatrixSV(&MStack_50,&local_c0,&local_b0);
  local_90.m[0]._0_4_ = SEXT24(local_b8.vx);
  local_a0._0_2_ = (short)local_a8 + local_b8.vx;
  local_9c = local_a4 + local_b8.vz;
  local_98._0_2_ = (short)local_a8 + local_b0.vx;
  local_94 = local_a4 + local_b0.vz;
  local_90.m[1]._2_4_ = (uint)(ushort)local_b0.vz << 0x10;
  local_a4 = (short)(param_3[2] >> 2) - local_a4;
  local_a8 = (*param_3 >> 2) - (int)(short)local_a8;
  local_9c = (short)(param_3[2] >> 2) - local_9c;
  local_a0 = (*param_3 >> 2) - (int)(short)local_a0;
  local_90.m[1][0] = local_b0.vx;
  local_98 = (*param_3 >> 2) - (int)(short)local_98;
  local_70.m._4_4_ = SEXT24((short)local_98);
  local_94 = (short)(param_3[2] >> 2) - local_94;
  local_70.m[0][0] = (short)local_a8;
  local_70.m[0][1] = (short)local_a0;
  local_70.m[1][1] = 0;
  local_70.m[1][2] = 0;
  local_70.m[2][0] = local_a4;
  local_70.m[2][1] = local_9c;
  local_70.m[2][2] = local_94;
  MulMatrix0(&local_90,&local_70,&local_90);
  bVar1 = false;
  if (((int)local_90.m[0][0] & 0x8000U) != ((int)local_90.m[0][1] & 0x8000U)) {
    bVar1 = ((int)local_90.m[1][0] & 0x8000U) != ((int)local_90.m[1][2] & 0x8000U);
  }
  return bVar1;
}


