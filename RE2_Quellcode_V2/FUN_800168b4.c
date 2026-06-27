/* FUN_800168b4 @ 0x800168b4  (Ghidra headless, raw MIPS overlay) */

void FUN_800168b4(short *param_1,short param_2,short param_3,int param_4)

{
  byte bVar1;
  undefined4 uVar2;
  MATRIX local_48;
  SVECTOR local_28;
  SVECTOR local_20;
  
  if ((param_1 != (short *)0x0) && ((char)param_1[7] != '\0')) {
    local_48.m[0][0] = 0x1000;
    local_48.m[0][1] = 0;
    local_48.m[0][2] = 0;
    local_48.m[1][0] = 0;
    local_48.m[1][1] = 0x1000;
    local_48.m[1][2] = 0;
    local_48.m[2][0] = 0;
    local_48.m[2][1] = 0;
    local_48.m[2][2] = 0x1000;
    local_48._18_2_ = 0;
    RotMatrixY((int)param_3,&local_48);
    local_28._0_4_ = SEXT24(*param_1);
    local_28.vz = param_1[1];
    ApplyMatrixSV(&local_48,&local_28,&local_20);
    param_1[8] = param_3;
    uVar2 = **(undefined4 **)(param_1 + 10);
    param_1[5] = param_2;
    param_1[4] = local_20.vx + (short)uVar2;
    uVar2 = *(undefined4 *)(*(int *)(param_1 + 10) + 8);
    bVar1 = *(byte *)(param_1 + 7);
    *(byte *)(param_1 + 7) = bVar1 | 4;
    param_1[6] = local_20.vz + (short)uVar2;
    if (param_4 != 0) {
      *(byte *)(param_1 + 7) = bVar1 & 0xfb;
    }
  }
  return;
}


