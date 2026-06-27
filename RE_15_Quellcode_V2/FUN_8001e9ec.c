void FUN_8001e9ec(uint *param_1)

{
  uint uVar1;
  MATRIX *m1;
  MATRIX local_48;
  VECTOR local_28;
  
  uVar1 = param_1[0x1b];
  m1 = (MATRIX *)(param_1 + 0x10);
  if ((*param_1 & 0x40) == 0) {
    FUN_80022da0(uVar1,param_1 + 6,m1);
  }
  if ((*param_1 & 0x800) != 0) {
    local_48.m[0]._0_4_ = param_1[6];
    local_48.m._4_4_ = param_1[7];
    local_48.m[1]._2_4_ = param_1[8];
    local_48.m[2]._0_4_ = param_1[9];
    local_48._16_4_ = param_1[10];
    local_48.t[0] = (int)((int)*(short *)(param_1[0x24] + 0x88) * param_1[0xb]) >> 0xc;
    local_48.t[1] = (int)((int)*(short *)(param_1[0x24] + 0x8a) * param_1[0xc]) >> 0xc;
    local_48.t[2] = (int)((int)*(short *)(param_1[0x24] + 0x8c) * param_1[0xd]) >> 0xc;
    if ((*param_1 & 0x40) == 0) {
      FUN_80022da0(uVar1,&local_48,m1);
    }
  }
  if ((*param_1 & 0x20) != 0) {
    FUN_8001f024(param_1,m1);
  }
  MulMatrix0((MATRIX *)&DAT_80076d14,m1,&local_48);
  SetLightMatrix(&local_48);
  if ((*param_1 & 0x1000) != 0) {
    FUN_80053fc0(param_1 + 0x15);
    MulMatrix0((MATRIX *)&DAT_80076d14,m1,&local_48);
    SetLightMatrix(&local_48);
    FUN_80053fc0(DAT_800ac784 + 0x34);
  }
  FUN_80022da0(&DAT_800b5288,m1,&local_48);
  if ((*param_1 & 0x400) != 0) {
    local_28.vx = (long)(short)param_1[0x22];
    local_28.vy = (long)*(short *)((int)param_1 + 0x8a);
    local_28.vz = (long)(short)param_1[0x23];
    ScaleMatrix(&local_48,&local_28);
  }
  if ((*param_1 & 0x2000) != 0) {
    local_28.vx = (long)(short)param_1[0x22];
    local_28.vy = (long)*(short *)((int)param_1 + 0x8a);
    local_28.vz = (long)(short)param_1[0x23];
    local_48.m[0][0] = 0x1000;
    local_48.m[0][1] = 0;
    local_48.m[0][2] = 0;
    local_48.m[1][0] = 0;
    local_48.m[1][1] = 0x1000;
    local_48.m[1][2] = 0;
    local_48.m[2][0] = 0;
    local_48.m[2][1] = 0;
    local_48._16_4_ = 0x1000;
    local_48.t[0] = 0;
    local_48.t[1] = 0;
    local_48.t[2] = 0;
    ScaleMatrix(&local_48,&local_28);
    MulMatrix0(&local_48,m1,&local_48);
    local_48.t[0] = param_1[0x15];
    local_48.t[1] = param_1[0x16];
    local_48.t[2] = param_1[0x17];
    local_28.vx = param_1[0x18];
    FUN_80022da0(&DAT_800b5288,&local_48,&local_48);
  }
  SetRotMatrix(&local_48);
  SetTransMatrix(&local_48);
  uVar1 = *param_1;
  if ((uVar1 & 1) != 0) {
    if ((uVar1 & 0x18) == 0) {
      FUN_800254a0(param_1[2],(uint)DAT_800aca34 * 0x28 + param_1[3],param_1 + 0x1a,0);
      FUN_800256b0(param_1[4],(uint)DAT_800aca34 * 0x34 + param_1[5],param_1 + 0x1a,0);
      if ((*param_1 & 0x80) != 0) {
        FUN_8002441c(param_1 + 1,param_1[0x25],param_1[0x26]);
      }
    }
    else if ((uVar1 & 8) == 0) {
      if ((uVar1 & 0x10) != 0) {
        FUN_80023a58(param_1[2],(uint)DAT_800aca34 * 0x28 + param_1[3],param_1 + 0x1a,param_1);
        FUN_80023e90(param_1[4],(uint)DAT_800aca34 * 0x34 + param_1[5],param_1 + 0x1a,param_1);
        FUN_8001f220(param_1);
      }
    }
    else {
      FUN_80023410(param_1[2],(uint)DAT_800aca34 * 0x28 + param_1[3],param_1);
      FUN_80023708(param_1[4],(uint)DAT_800aca34 * 0x34 + param_1[5],param_1);
      FUN_8001f1e0(param_1);
    }
  }
  return;
}
