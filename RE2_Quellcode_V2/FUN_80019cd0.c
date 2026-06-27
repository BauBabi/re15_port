/* FUN_80019cd0 @ 0x80019cd0  (Ghidra headless, raw MIPS overlay) */

void FUN_80019cd0(uint *param_1,uint *param_2)

{
  int iVar1;
  MATRIX *r0;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  MATRIX *r0_00;
  MATRIX MStack_110;
  MATRIX MStack_f0;
  MATRIX local_d0;
  MATRIX local_b0;
  uint local_90;
  uint local_8c;
  uint local_88;
  uint local_84;
  uint local_80;
  int local_7c;
  int local_78;
  int local_74;
  VECTOR local_30;
  
  if ((*param_2 & 1) != 0) {
    FUN_80076f88(param_1[0x66] + 0x5c);
    SetColorMatrix((MATRIX *)&DAT_8009db84);
    FUN_8002ce94(&DAT_800dcba8,&DAT_8009db44,&MStack_110);
    r0_00 = &MStack_f0;
    FUN_8002ce94(&DAT_8009db64,&DAT_8009db44,r0_00);
    if ((*param_2 & 0x400) != 0) {
      local_d0.m[0]._0_4_ = param_2[0x12];
      local_d0.m._4_4_ = param_2[0x13];
      local_d0.m[1]._2_4_ = param_2[0x14];
      local_d0.m[2]._0_4_ = param_2[0x15];
      local_d0._16_4_ = param_2[0x16];
      local_d0.t[0] = param_2[0x17];
      local_d0.t[1] = param_2[0x18];
      local_d0.t[2] = param_2[0x19];
      local_30.vx = (long)(short)param_2[0x23];
      local_30.vy = (long)*(short *)((int)param_2 + 0x8e);
      local_30.vz = (long)(short)param_2[0x24];
      ScaleMatrix(&local_d0,&local_30);
      TransposeMatrix((MATRIX *)(param_2 + 0x12),&local_b0);
      local_b0.t[0] = 0;
      local_b0.t[1] = 0;
      local_b0.t[2] = 0;
      local_90 = DAT_8009db44;
      local_8c = DAT_8009db48;
      local_88 = DAT_8009db4c;
      local_84 = DAT_8009db50;
      local_80 = DAT_8009db54;
      local_7c = -local_d0.t[0];
      local_74 = -local_d0.t[2];
      local_78 = -local_d0.t[1];
      FUN_8002ce94(&local_b0,&local_90,&local_b0);
      FUN_8002ce94(&local_d0,&local_b0,&local_d0);
      FUN_8002ce94(&DAT_800dcba8,&local_d0,&MStack_110);
    }
    if ((*param_2 & 0x2000) != 0) {
      local_d0.m[0][0] = (undefined2)DAT_8009db44;
      local_d0.m[0][1] = DAT_8009db44._2_2_;
      local_d0.m[0][2] = (undefined2)DAT_8009db48;
      local_d0.m[1][0] = DAT_8009db48._2_2_;
      local_d0.m[1][1] = (undefined2)DAT_8009db4c;
      local_d0.m[1][2] = DAT_8009db4c._2_2_;
      local_d0.m[2][0] = (undefined2)DAT_8009db50;
      local_d0.m[2][1] = DAT_8009db50._2_2_;
      local_d0._16_4_ = DAT_8009db54;
      local_d0.t[0] = param_2[0x17];
      local_d0.t[1] = param_2[0x18];
      local_d0.t[2] = param_2[0x19];
      local_30.vx = (long)(short)param_2[0x23];
      local_30.vy = (long)*(short *)((int)param_2 + 0x8e);
      local_30.vz = (long)(short)param_2[0x24];
      ScaleMatrix(&local_d0,&local_30);
      local_b0.m[0][0] = (undefined2)DAT_8009db44;
      local_b0.m[0][1] = DAT_8009db44._2_2_;
      local_b0.m[0][2] = (undefined2)DAT_8009db48;
      local_b0.m[1][0] = DAT_8009db48._2_2_;
      local_b0.m[1][1] = (undefined2)DAT_8009db4c;
      local_b0.m[1][2] = DAT_8009db4c._2_2_;
      local_b0.m[2][0] = (undefined2)DAT_8009db50;
      local_b0.m[2][1] = DAT_8009db50._2_2_;
      local_b0._16_4_ = DAT_8009db54;
      local_b0.t[0] = -local_d0.t[0];
      local_b0.t[2] = -local_d0.t[2];
      local_b0.t[1] = -local_d0.t[1];
      FUN_8002ce94(&local_d0,&local_b0,&local_d0);
      FUN_8002ce94(&DAT_800dcba8,&local_d0,&MStack_110);
    }
    iVar1 = FUN_8002c820(param_1 + 0xe,DAT_800ce338);
    if ((iVar1 != 0) && (r0 = &MStack_110, (*param_1 & 0x80000) == 0)) {
      gte_SetRotMatrix(r0);
      gte_SetTransMatrix(r0);
      gte_SetLightMatrix(r0_00);
      uVar2 = (uint)DAT_800ce5e0;
      uVar6 = param_2[0x1c];
      uVar3 = param_2[5];
      uVar4 = param_2[4];
      uVar5 = (param_2[1] & 2) >> 1;
      FUN_80027bec(param_2[2],param_2[3] + uVar2 * 0x28,uVar6,uVar5);
      FUN_80027dbc(uVar4,uVar3 + uVar2 * 0x34,uVar6,uVar5);
    }
  }
  return;
}


