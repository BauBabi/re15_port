/* FUN_80076640 @ 0x80076640  (Ghidra headless, raw MIPS overlay) */

void FUN_80076640(int param_1,int param_2,int param_3)

{
  int iVar1;
  MATRIX *m1;
  MATRIX MStack_60;
  MATRIX MStack_40;
  SVECTOR local_20;
  SVECTOR local_18;
  
  if (param_2 == 0) {
    local_18.vx = 0;
    local_18.vy = 0;
    local_18.vz = DAT_800d5c20 + DAT_800d5c01 * 0x555 + (short)param_3 * -0x555;
    RotMatrix(&local_18,&MStack_60);
    local_20.vx = 0x1ae;
    local_20.vy = 0;
    local_20.vz = 0;
    ApplyMatrix(&MStack_60,&local_20,(VECTOR *)&local_20);
    *(int *)(param_1 + 0x28) = local_20.vx + DAT_800ab1f4;
    *(int *)(param_1 + 0x2c) = local_20.vy + DAT_800ab1f8;
    *(int *)(param_1 + 0x30) = local_20.vz + DAT_800ab1fc;
    if (DAT_800d5bf2 == 8) {
      *(short *)(param_1 + 0x66) = *(short *)(param_1 + 0x66) + 0x26;
    }
    if (DAT_800d5bf2 == 9) {
      *(short *)(param_1 + 0x66) = *(short *)(param_1 + 0x66) + -0x26;
    }
    if (DAT_800d5bf2 == 0xb) {
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + 0x26;
    }
    if (DAT_800d5bf2 == 0xc) {
      *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + -0x26;
    }
    if (DAT_800d5bf2 < 9) {
      if (-1 < *(int *)(param_1 + 0x28)) {
        iVar1 = *(int *)(param_1 + 0x28) + (DAT_800d5c1e + 2) * -2;
        *(int *)(param_1 + 0x28) = iVar1;
        if (0 < iVar1) goto LAB_800768c8;
        *(undefined4 *)(param_1 + 0x28) = 0;
      }
      iVar1 = *(int *)(param_1 + 0x28) + (DAT_800d5c1e + 2) * 2;
      *(int *)(param_1 + 0x28) = iVar1;
      if (iVar1 < 0) goto LAB_800768c8;
    }
    *(undefined4 *)(param_1 + 0x28) = 0;
  }
  else {
    local_20.vx = 0;
    local_18.vx = 0;
    local_20.vy = 0;
    local_18.vy = 0;
    local_20.vz = 0;
    local_18.vz = 0;
    RotMatrix(&local_18,&MStack_60);
    ApplyMatrix(&MStack_60,&local_20,(VECTOR *)&local_20);
    *(int *)(param_1 + 0x28) = ((local_20.vx + DAT_800ab1f4) - param_3) + -0x17;
    *(int *)(param_1 + 0x2c) = local_20.vy + DAT_800ab1f8 + param_3 * 0x3e + -0xde;
    *(int *)(param_1 + 0x30) = local_20.vz + DAT_800ab1fc + 0xe9;
  }
LAB_800768c8:
  RotMatrix((SVECTOR *)(param_1 + 100),(MATRIX *)(param_1 + 0x14));
  m1 = (MATRIX *)(param_1 + 0x44);
  FUN_8002ce94(*(undefined4 *)(param_1 + 0x70),(MATRIX *)(param_1 + 0x14),m1);
  FUN_80076f88(param_1 + 0x58);
  SetColorMatrix((MATRIX *)&DAT_8009db84);
  MulMatrix0((MATRIX *)&DAT_8009db64,m1,&MStack_40);
  SetLightMatrix(&MStack_40);
  FUN_8002ce94(&DAT_800dcba8,m1,&MStack_40);
  SetRotMatrix(&MStack_40);
  SetTransMatrix(&MStack_40);
  FUN_80027bec(*(undefined4 *)(param_1 + 4),*(int *)(param_1 + 8) + (uint)DAT_800ce5e0 * 0x28,
               *(undefined4 *)(param_1 + 0x6c),0);
  FUN_80027dbc(*(undefined4 *)(param_1 + 0xc),*(int *)(param_1 + 0x10) + (uint)DAT_800ce5e0 * 0x34,
               *(undefined4 *)(param_1 + 0x6c),0);
  return;
}


