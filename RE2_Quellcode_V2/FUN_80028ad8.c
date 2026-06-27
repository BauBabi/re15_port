/* FUN_80028ad8 @ 0x80028ad8  (Ghidra headless, raw MIPS overlay) */

void FUN_80028ad8(uint *param_1,int param_2)

{
  uint *puVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  MATRIX MStack_a0;
  SVECTOR local_80;
  MATRIX aMStack_78 [3];
  
  puVar1 = DAT_800ce330;
  sVar2 = *(short *)((int)param_1 + 0x86);
  if (sVar2 == 0) {
    local_80.vy = (short)param_1[0xe];
    local_80.vz = 0;
    local_80.vx = 0;
    RotMatrix(&local_80,&MStack_a0);
    local_80.vx = (short)param_1[0xf];
    local_80.vy = *(short *)((int)param_1 + 0x3a);
    ApplyMatrixSV(&MStack_a0,&local_80,(SVECTOR *)(param_1 + 0xe));
    *(undefined2 *)((int)param_1 + 0x86) = 3;
    *(undefined1 *)((int)param_1 + 0x7a) = 0xf;
    sVar2 = *(short *)((int)param_1 + 0x86);
  }
  if (1 < sVar2) {
    iVar3 = FUN_8004fba0(param_1 + 0x17,100,0x2000,1);
    *(int *)(param_2 + 0x14) = *(int *)(param_2 + 0x14) + (int)(short)param_1[0xe];
    sVar2 = *(short *)((int)param_1 + 0x3a) + (short)*(char *)((int)param_1 + 0x79);
    *(short *)((int)param_1 + 0x3a) = sVar2;
    *(int *)(param_2 + 0x18) = *(int *)(param_2 + 0x18) + (int)sVar2;
    *(int *)(param_2 + 0x1c) = *(int *)(param_2 + 0x1c) + (int)(short)param_1[0xf];
    if (iVar3 < *(int *)(param_2 + 0x18)) {
      *(short *)((int)param_1 + 0x86) = *(short *)((int)param_1 + 0x86) + -1;
      *(int *)(param_2 + 0x18) = iVar3;
      *(short *)((int)param_1 + 0x3a) = -*(short *)((int)param_1 + 0x3a) >> 2;
      *(short *)(param_1 + 0xe) = (short)param_1[0xe] >> 2;
      *(short *)(param_1 + 0xf) = (short)param_1[0xf] >> 2;
      iVar4 = FUN_80015fe8();
      FUN_8001bf10((*puVar1 >> 0x13 & 0x18) << 0x10 | 2000,iVar4 << 4,param_1 + 0x12,0);
      if ((*param_1 & 0x10) != 0) {
        *param_1 = *param_1 | 0x2000;
        *(undefined2 *)(param_1 + 0x23) = 7000;
        *(undefined2 *)(param_1 + 0x24) = 7000;
        *(undefined2 *)((int)param_1 + 0x8e) = 0;
        *(undefined2 *)((int)param_1 + 0x86) = 1;
      }
      if (*(short *)((int)param_1 + 0x86) == 1) {
        *(int *)(param_2 + 0x18) = iVar3;
        *param_1 = *param_1 & 0xfffffffe;
      }
    }
    iVar3 = FUN_8004c1bc(param_1 + 0x17,100,1 << (-((int)param_1[0x18] / 0x708) & 0x1fU),0x2000);
    if (iVar3 != 0) {
      *(short *)(param_1 + 0xe) = (short)param_1[0xe] >> 2;
      *(short *)(param_1 + 0xf) = (short)param_1[0xf] >> 2;
      *(short *)(param_1 + 0xe) = -(short)param_1[0xe];
      *(short *)(param_1 + 0xf) = -(short)param_1[0xf];
    }
  }
  if (-1 < *(char *)((int)param_1 + 0x7a)) {
    local_80.vx = *(short *)((int)param_1 + 0x3e);
    local_80.vy = (short)param_1[0x10];
    local_80.vz = *(short *)((int)param_1 + 0x42);
    RotMatrix(&local_80,aMStack_78);
    FUN_80028f48(param_1 + 0x12,aMStack_78,param_1 + 0x12,
                 *(char *)((int)param_1 + 0x7a) * -0x100 + 0x1000);
    *(char *)((int)param_1 + 0x7a) = *(char *)((int)param_1 + 0x7a) + -1;
  }
  return;
}


