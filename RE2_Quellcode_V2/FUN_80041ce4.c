/* FUN_80041ce4 @ 0x80041ce4  (Ghidra headless, raw MIPS overlay) */

bool FUN_80041ce4(int param_1,int param_2,short *param_3)

{
  bool bVar1;
  MATRIX *m;
  SVECTOR local_90;
  SVECTOR local_88;
  SVECTOR local_80;
  uint local_78;
  short local_74;
  int local_70;
  short local_6c;
  int local_68;
  short local_64;
  MATRIX local_60;
  MATRIX local_40;
  
  m = (MATRIX *)(param_1 + 0x14);
  local_60.t[0] = (long)*param_3;
  local_60.m[0][0] = 0x1000;
  local_60.m[0][1] = 0;
  local_60.m[0][2] = 0;
  local_60.m[1][0] = 0;
  local_60.m[1][1] = 0x1000;
  local_60.m[1][2] = 0;
  local_60.m[2][0] = 0;
  local_60.m[2][1] = 0;
  local_60.m[2][2] = 0x1000;
  local_60._18_2_ = 0;
  local_60.t[1] = 0;
  local_60.t[2] = param_3[3] * -4 - (int)param_3[1];
  FUN_8002ce94(m,&local_60,&local_40);
  local_78 = local_40.t[0] >> 2 & 0xffff;
  local_74 = (short)(local_40.t[2] >> 2);
  local_90._0_4_ = SEXT24(param_3[2]);
  local_90.vz = 0;
  local_90.pad = 0;
  ApplyMatrixSV(m,&local_90,&local_88);
  local_90.vx = 0;
  local_90.vy = 0;
  local_90._4_4_ = (int)param_3[3] << 1;
  ApplyMatrixSV(m,&local_90,&local_80);
  local_60.m[0]._0_4_ = SEXT24(local_88.vx);
  local_70._0_2_ = (short)local_78 + local_88.vx;
  local_6c = local_74 + local_88.vz;
  local_68._0_2_ = (short)local_78 + local_80.vx;
  local_64 = local_74 + local_80.vz;
  local_60.m[1]._2_4_ = (uint)(ushort)local_80.vz << 0x10;
  local_74 = (short)(*(int *)(param_2 + 0x30) >> 2) - local_74;
  local_78 = (*(int *)(param_2 + 0x28) >> 2) - (int)(short)local_78;
  local_6c = (short)(*(int *)(param_2 + 0x30) >> 2) - local_6c;
  local_70 = (*(int *)(param_2 + 0x28) >> 2) - (int)(short)local_70;
  local_60.m[1][0] = local_80.vx;
  local_68 = (*(int *)(param_2 + 0x28) >> 2) - (int)(short)local_68;
  local_40.m._4_4_ = SEXT24((short)local_68);
  local_64 = (short)(*(int *)(param_2 + 0x30) >> 2) - local_64;
  local_40.m[0][0] = (short)local_78;
  local_40.m[0][1] = (short)local_70;
  local_40.m[1][1] = 0;
  local_40.m[1][2] = 0;
  local_40.m[2][0] = local_74;
  local_40.m[2][1] = local_6c;
  local_40.m[2][2] = local_64;
  MulMatrix0(&local_60,&local_40,&local_60);
  bVar1 = false;
  if (((int)local_60.m[0][0] & 0x8000U) != ((int)local_60.m[0][1] & 0x8000U)) {
    bVar1 = ((int)local_60.m[1][0] & 0x8000U) != ((int)local_60.m[1][2] & 0x8000U);
  }
  return bVar1;
}


