void FUN_800245d8(short param_1)

{
  int iVar1;
  SVECTOR *r0;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined1 *puVar5;
  undefined1 auStack_40 [16];
  undefined2 local_30;
  undefined2 local_2e;
  undefined2 local_2c;
  MATRIX local_28;
  
  puVar5 = auStack_40;
  local_30 = *(undefined2 *)(DAT_800ac784 + 0x8c);
  local_2c = 0;
  local_2e = 0;
  local_28.m[0][0] = 0x1000;
  local_28.m[0][1] = 0;
  local_28.m[0][2] = 0;
  local_28.m[1][0] = 0;
  local_28.m[1][1] = 0x1000;
  local_28.m[1][2] = 0;
  local_28.m[2][0] = 0;
  local_28.m[2][1] = 0;
  local_28.m[2][2] = 0x1000;
  local_28._18_2_ = 0;
  local_28.t[0] = 0;
  local_28.t[1] = 0;
  local_28.t[2] = 0;
  RotMatrixY((int)*(short *)(DAT_800ac784 + 0x6a) + (int)param_1,&local_28);
  gte_SetRotMatrix(&local_28);
  r0 = (SVECTOR *)(puVar5 + 0x10);
  gte_ldv0(r0);
  iVar1 = DAT_800ac784;
  gte_rtv0();
  uVar2 = gte_stIR1();
  uVar3 = gte_stIR2();
  uVar4 = gte_stIR3();
  r0->vx = (short)uVar2;
  r0->vy = (short)uVar3;
  r0->vz = (short)uVar4;
  *(int *)(iVar1 + 0x34) = (int)*(short *)(puVar5 + 0x10) + *(int *)(iVar1 + 0x34);
  *(int *)(iVar1 + 0x3c) = (int)*(short *)(puVar5 + 0x14) + *(int *)(iVar1 + 0x3c);
  return;
}
