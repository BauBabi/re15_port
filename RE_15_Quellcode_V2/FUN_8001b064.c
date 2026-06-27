void FUN_8001b064(int param_1,short param_2)

{
  undefined1 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  MATRIX local_88;
  MATRIX MStack_68;
  SVECTOR local_48;
  undefined1 local_40 [8];
  undefined1 auStack_38 [8];
  undefined1 local_30 [8];
  long alStack_28 [2];
  
  local_88.m[0][0] = 0x1000;
  local_88.m[0][1] = 0;
  local_88.m[0][2] = 0;
  local_88.m[1][0] = 0;
  local_88.m[1][1] = 0x1000;
  local_88.m[1][2] = 0;
  local_88.m[2][0] = 0;
  local_88.m[2][1] = 0;
  local_88.m[2][2] = 0x1000;
  local_88._18_2_ = 0;
  local_88.t[0] = 0;
  local_88.t[1] = 0;
  local_88.t[2] = 0;
  RotMatrixY((int)*(short *)(DAT_800ac784 + 0x6a),&local_88);
  local_48.vy = 0;
  local_48.vx = *(ushort *)(param_1 + 8);
  local_48.vz = *(short *)(param_1 + 10);
  ApplyMatrixSV(&local_88,&local_48,(SVECTOR *)local_40);
  local_88.t[1] = (long)param_2;
  local_88.t[0] = (int)(short)local_40._0_2_ + *(int *)(DAT_800ac784 + 0x34);
  local_88.t[2] = (int)(short)local_40._4_2_ + *(int *)(DAT_800ac784 + 0x3c);
  FUN_80022da0(&DAT_800b5288,&local_88);
  SetRotMatrix(&MStack_68);
  SetTransMatrix(&MStack_68);
  local_40._0_2_ = *(ushort *)(param_1 + 0xc);
  local_48.vx = ~local_40._0_2_ - 1;
  local_48.vy = 0;
  local_48.vz = *(undefined2 *)(param_1 + 0xe);
  puVar1 = local_40 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 |
       (uint)((ushort)local_48.vx >> (3 - uVar3) * 8);
  puVar1 = local_40 + 7;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | (uint)local_48._4_4_ >> (3 - uVar3) * 8;
  local_40._4_2_ = local_48.vz;
  local_40._6_2_ = local_48.pad;
  local_40._2_2_ = 0;
  puVar1 = auStack_38 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 |
       (uint)((ushort)local_48.vx >> (3 - uVar3) * 8);
  auStack_38._0_2_ = local_48.vx;
  auStack_38._2_2_ = 0;
  puVar1 = auStack_38 + 7;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | (uint)local_48._4_4_ >> (3 - uVar3) * 8;
  auStack_38._6_2_ = local_48.pad;
  auStack_38._4_2_ = ~*(ushort *)(param_1 + 0xe) - 1;
  puVar1 = local_30 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 |
       (uint)((ushort)local_48.vx >> (3 - uVar3) * 8);
  local_30._0_2_ = local_48.vx;
  local_30._2_2_ = 0;
  puVar1 = local_30 + 7;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 |
       (uint)auStack_38._4_4_ >> (3 - uVar3) * 8;
  local_30._4_4_ = auStack_38._4_4_;
  local_30._2_2_ = 0;
  local_30._0_2_ = local_40._0_2_;
  iVar2 = FUN_80014368(DAT_800ac784 + 0x34,DAT_800ac790);
  if (iVar2 != 0) {
    uVar3 = RotAverage4(&local_48,(SVECTOR *)local_40,(SVECTOR *)auStack_38,(SVECTOR *)local_30,
                        (long *)((uint)DAT_800aca34 * 0x28 + param_1 + 0x18),
                        (long *)((uint)DAT_800aca34 * 0x28 + param_1 + 0x20),
                        (long *)((uint)DAT_800aca34 * 0x28 + param_1 + 0x28),
                        (long *)((uint)DAT_800aca34 * 0x28 + param_1 + 0x30),alStack_28,alStack_28);
    iVar2 = (uVar3 >> 4) * 4;
    iVar4 = (uint)DAT_800aca34 * 0x28 + param_1;
                    /* Probable PsyQ macro: addPrim(). */
    *(uint *)(iVar4 + 0x10) =
         *(uint *)(iVar4 + 0x10) & 0xff000000 |
         *(uint *)(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000 + iVar2) & 0xffffff;
    *(uint *)(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000 + iVar2) =
         *(uint *)(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000 + iVar2) & 0xff000000 |
         param_1 + (uint)DAT_800aca34 * 0x28 + 0x10 & 0xffffff;
  }
  return;
}
