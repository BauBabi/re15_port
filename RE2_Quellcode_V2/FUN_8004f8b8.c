/* FUN_8004f8b8 @ 0x8004f8b8  (Ghidra headless, raw MIPS overlay) */

int FUN_8004f8b8(int param_1,int param_2,int param_3,short *param_4)

{
  ushort uVar1;
  bool bVar2;
  uint uVar3;
  
  uVar3 = ((ushort)param_4[5] & 0x30) >> 4;
  if (uVar3 == 1) {
    param_2 = *param_4 - param_1;
    uVar1 = param_4[2];
LAB_8004f9d0:
    uVar3 = (uint)uVar1;
    param_3 = (param_2 * -0x708) / (int)uVar3;
    if (uVar3 == 0) {
      trap(0x1c00);
    }
    if ((uVar3 == 0xffffffff) && (param_2 * -0x708 == -0x80000000)) {
      trap(0x1800);
    }
    bVar2 = param_3 < 0x709;
  }
  else {
    if (uVar3 < 2) {
      bVar2 = param_3 < 0x709;
      if (uVar3 != 0) goto LAB_8004fa08;
      uVar3 = (uint)(ushort)param_4[2];
      param_1 = param_1 - ((int)*param_4 + uVar3);
    }
    else {
      if (uVar3 != 2) {
        bVar2 = param_3 < 0x709;
        if (uVar3 != 3) goto LAB_8004fa08;
        param_2 = param_4[1] - param_2;
        uVar1 = param_4[3];
        goto LAB_8004f9d0;
      }
      uVar3 = (uint)(ushort)param_4[3];
      param_1 = param_2 - ((int)param_4[1] + uVar3);
    }
    param_3 = (param_1 * -0x708) / (int)uVar3;
    if (uVar3 == 0) {
      trap(0x1c00);
    }
    if ((uVar3 == 0xffffffff) && (param_1 * -0x708 == -0x80000000)) {
      trap(0x1800);
    }
    bVar2 = param_3 < 0x709;
  }
LAB_8004fa08:
  if (!bVar2) {
    param_3 = 0x708;
  }
  if (param_3 < 0) {
    param_3 = 0;
  }
  return param_3;
}


