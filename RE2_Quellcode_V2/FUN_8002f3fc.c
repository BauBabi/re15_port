/* FUN_8002f3fc @ 0x8002f3fc  (Ghidra headless, raw MIPS overlay) */

void FUN_8002f3fc(int param_1,uint param_2,uint param_3,int param_4)

{
  int iVar1;
  uint *puVar2;
  ushort *puVar3;
  byte *pbVar4;
  uint uVar5;
  byte *pbVar6;
  uint uVar7;
  uint uVar8;
  undefined *puVar9;
  SVECTOR *r0;
  uint unaff_s1;
  uint *puVar10;
  uint *puVar11;
  int iVar12;
  uint uVar13;
  undefined1 *puVar14;
  uint uVar15;
  undefined1 auStack_c8 [16];
  undefined2 local_b8;
  undefined2 local_b6;
  short local_b4;
  short local_b2;
  VECTOR local_b0;
  SVECTOR local_a0;
  SVECTOR local_98 [3];
  MATRIX local_80;
  MATRIX MStack_60;
  
  puVar14 = auStack_c8;
  local_b4 = (short)param_2 * 0x708 + -1;
  local_b2 = (short)param_3 * 0x708 + -1;
  local_b8 = 0;
  local_b6 = 0;
  local_a0.vy = 0;
  local_a0.vx = (short)*(undefined4 *)(param_4 + 0x38) - (short)*(undefined4 *)(param_1 + 0x68);
  local_a0.vz = (short)*(undefined4 *)(param_4 + 0x40) - (short)*(undefined4 *)(param_1 + 0x70);
  local_80.m[0]._0_4_ = DAT_8009db44;
  local_80.m._4_4_ = DAT_8009db48;
  local_80.m[1]._2_4_ = DAT_8009db4c;
  local_80.m[2]._0_4_ = DAT_8009db50;
  local_80._16_4_ = DAT_8009db54;
  local_80.t[0] = DAT_8009db58;
  local_80.t[1] = DAT_8009db5c;
  local_80.t[2] = DAT_8009db60;
  RotMatrixY(-(int)*(short *)(param_1 + 0x76),&local_80);
  ApplyMatrix(&local_80,&local_a0,&local_b0);
  iVar1 = FUN_8002c904(&local_b0,&local_b8);
  if (iVar1 != 0) {
    iVar12 = 0;
    iVar1 = 0;
    uVar5 = 0;
    puVar3 = (ushort *)
             (&DAT_8009dcd0 + (uint)(byte)(&DAT_8009dcc0)[*(ushort *)(param_1 + 0x10e)] * 2);
    if (param_3 != 0) {
      do {
        unaff_s1 = 0;
        if (param_2 != 0) {
          do {
            if (((uint)*puVar3 & 0x8000 >> (unaff_s1 & 0x1f)) != 0) {
              iVar1 = iVar1 + 1;
            }
            if (iVar12 == local_b0.vx / 0x708 + (local_b0.vz / 0x708) * param_2) goto LAB_8002f5e4;
            unaff_s1 = unaff_s1 + 1;
            iVar12 = iVar12 + 1;
          } while (unaff_s1 < param_2);
        }
        uVar5 = uVar5 + 1;
        puVar3 = puVar3 + 1;
      } while (uVar5 < param_3);
    }
LAB_8002f5e4:
    if (((uint)*puVar3 & 0x8000 >> (unaff_s1 & 0x1f)) != 0) {
      puVar10 = (uint *)(*(int *)(param_1 + 0x20) + (uint)DAT_800ce5e0 * 0x34 + (iVar1 + -1) * 0x68)
      ;
      if (((*puVar10 & 0xc000000) != 0) && (DAT_800d8cce < DAT_800d8ccd)) {
        iVar1 = (local_b0.vx / 0x708) * 0x708;
        iVar12 = (local_b0.vz / 0x708) * 0x708;
        uVar15 = (uint)*(ushort *)((int)puVar10 + 0xe);
        uVar13 = (uint)*(ushort *)((int)puVar10 + 0x1a);
        *puVar10 = *puVar10 & 0xffffff;
        local_80.m[0][0] = (short)DAT_8009db44;
        local_80.m[0][1] = DAT_8009db44._2_2_;
        local_80.m[0][2] = (short)DAT_8009db48;
        local_80.m[1][0] = DAT_8009db48._2_2_;
        local_80.m[1][1] = (short)DAT_8009db4c;
        local_80.m[1][2] = DAT_8009db4c._2_2_;
        local_80.m[2][0] = (short)DAT_8009db50;
        local_80.m[2][1] = DAT_8009db50._2_2_;
        local_80.m[2][2] = (short)DAT_8009db54;
        local_80._18_2_ = DAT_8009db54._2_2_;
        local_80.t[0] = DAT_8009db58;
        local_80.t[1] = DAT_8009db5c;
        local_80.t[2] = DAT_8009db60;
        puVar11 = (uint *)(DAT_800d8cc4 + (uint)DAT_800ce5e0 * 0x34 + (uint)DAT_800d8cce * 0x548);
        DAT_800d8cce = DAT_800d8cce + 1;
        RotMatrix((SVECTOR *)(param_1 + 0x74),&MStack_60);
        MStack_60.t[0] = *(long *)(param_1 + 0x68);
        MStack_60.t[1] = *(long *)(param_1 + 0x6c);
        MStack_60.t[2] = *(long *)(param_1 + 0x70);
        FUN_8002ce94(&DAT_800dcba8,&MStack_60,&local_80);
        SetRotMatrix(&local_80);
        SetTransMatrix(&local_80);
        pbVar6 = &DAT_8009ddcc;
        uVar5 = 0;
        r0 = local_98;
        uVar7 = 0xffffff;
        uVar8 = 0xff000000;
        puVar10 = puVar11 + 0xb;
        pbVar4 = &DAT_8009ddd3;
        puVar9 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
        do {
          *(uint *)(puVar14 + 0x28) = iVar1 + (uint)*pbVar6 * 100;
          *(uint *)(puVar14 + 0x2c) = iVar12 + (uint)pbVar4[-6] * 100;
          *(uint *)(puVar14 + 0x30) = iVar1 + (uint)pbVar4[-5] * 100;
          gte_ldv0((SVECTOR *)(puVar14 + 0x28));
          *(uint *)(puVar14 + 0x34) = iVar12 + (uint)pbVar4[-4] * 100;
          gte_rtps_b();
          *(uint *)(puVar14 + 0x38) = iVar1 + (uint)pbVar4[-3] * 100;
          *(uint *)(puVar14 + 0x3c) = iVar12 + (uint)pbVar4[-2] * 100;
          *(uint *)(puVar14 + 0x40) = iVar1 + (uint)pbVar4[-1] * 100;
          *(uint *)(puVar14 + 0x44) = iVar12 + (uint)*pbVar4 * 100;
          gte_stsxy((long *)(puVar11 + 2));
          gte_stszotz((long *)(puVar14 + 0x88));
          gte_ldv3(r0,(SVECTOR *)(puVar14 + 0x38),(SVECTOR *)(puVar14 + 0x40));
          *(short *)((int)puVar10 + -0x1e) = (short)uVar15;
          *(short *)((int)puVar10 + -0x12) = (short)uVar13;
          gte_rtpt_b();
          pbVar4 = pbVar4 + 8;
          pbVar6 = pbVar6 + 8;
          uVar5 = uVar5 + 1;
          gte_stsxy3((long *)(puVar11 + 5),(long *)(puVar11 + 8),(long *)puVar10);
          gte_stszotz((long *)(puVar14 + 0x8c));
          puVar10 = puVar10 + 0x1a;
          puVar2 = (uint *)(puVar9 + (*(int *)(puVar14 + 0x88) >> 3) * 4);
          *puVar11 = *puVar11 & uVar8 | *puVar2 & uVar7;
          *puVar2 = *puVar2 & uVar8 | (uint)puVar11 & uVar7;
          puVar11 = puVar11 + 0x1a;
        } while (uVar5 < 0xd);
      }
    }
  }
  return;
}


