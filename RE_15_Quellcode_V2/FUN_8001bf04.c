undefined4 FUN_8001bf04(int *param_1,short *param_2,ushort *param_3)

{
  uint uVar1;
  undefined4 uVar2;
  
  uVar1 = (((int)*param_2 + *param_1) - ((int)(short)param_3[2] + (uint)*param_3) & 0x80000000 |
          (((int)*param_2 + *param_1) - (int)(short)param_3[2] & 0x80000000U) >> 1) >> 0x1e;
  uVar2 = 0xf00;
  if (uVar1 == 2) {
    uVar1 = (((int)param_2[2] + param_1[2]) - ((int)(short)param_3[3] + (uint)param_3[1]) &
             0x80000000 |
            (((int)param_2[2] + param_1[2]) - (int)(short)param_3[3] & 0x80000000U) >> 1) >> 0x1e;
    if (uVar1 != 2) {
      if (uVar1 < 3) {
        if (uVar1 == 0) {
          uVar2 = 0x400;
        }
      }
      else if (uVar1 == 3) {
        uVar2 = 0xc00;
      }
    }
  }
  else if (uVar1 < 3) {
    if (uVar1 == 0) {
      uVar2 = 0x800;
    }
  }
  else if (uVar1 == 3) {
    uVar2 = 0;
  }
  return uVar2;
}
