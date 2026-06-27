void FUN_8001f024(uint *param_1,MATRIX *param_2)

{
  short sVar1;
  int iVar2;
  MATRIX local_30;
  
  param_2->t[0] = (int)(short)param_1[0xe] + param_2->t[0];
  sVar1 = (short)*(char *)((int)param_1 + 0x75) + *(short *)((int)param_1 + 0x3a);
  *(short *)((int)param_1 + 0x3a) = sVar1;
  param_2->t[1] = (int)sVar1 + param_2->t[1];
  param_2->t[2] = (int)(short)param_1[0xf] + param_2->t[2];
  if ((int)*(short *)(DAT_800ac784 + 0x1ba) < param_2->t[1]) {
    param_2->t[1] = (int)*(short *)(DAT_800ac784 + 0x1ba);
    *(short *)((int)param_1 + 0x3a) = -*(short *)((int)param_1 + 0x3a) >> 3;
    if ((*param_1 & 0x10) != 0) {
      *param_1 = *param_1 | 0x2000;
      *(undefined2 *)(param_1 + 0x22) = 10000;
      *(undefined2 *)(param_1 + 0x23) = 8000;
      *(undefined2 *)((int)param_1 + 0x8a) = 0;
      *param_1 = *param_1 & 0xffffffef;
    }
  }
  if ((*(short *)((int)param_1 + 0x3a) == 0) && (param_2->t[1] == 0)) {
    *(undefined2 *)(param_1 + 0x15) = 1;
    *param_1 = *param_1 & 0xfffffffe;
  }
  if (*(short *)((int)param_1 + 0x82) != 0) {
    local_30.m[0][0] = 0x1000;
    local_30.m[0][1] = 0;
    local_30.m[0][2] = 0;
    local_30.m[1][0] = 0;
    local_30.m[1][1] = 0x1000;
    local_30.m[1][2] = 0;
    local_30.m[2][0] = 0;
    local_30.m[2][1] = 0;
    local_30.m[2][2] = 0x1000;
    local_30._18_2_ = 0;
    local_30.t[0] = 0;
    local_30.t[1] = 0;
    local_30.t[2] = 0;
    iVar2 = (int)*(char *)((int)param_1 + 0x76) + (uint)*(ushort *)((int)param_1 + 0x82);
    *(short *)((int)param_1 + 0x82) = (short)iVar2;
    RotMatrixZ(iVar2 * 0x10000 >> 0x10,&local_30);
    MulMatrix0(param_2,&local_30,param_2);
  }
  return;
}
