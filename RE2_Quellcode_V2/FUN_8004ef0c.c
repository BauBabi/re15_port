/* FUN_8004ef0c @ 0x8004ef0c  (Ghidra headless, raw MIPS overlay) */

uint FUN_8004ef0c(int *param_1,undefined4 param_2,ushort *param_3)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  
  uVar1 = param_3[3];
  uVar3 = (uint)(*param_1 -
                ((int)((((uint)param_3[2] + (uint)*param_3) - (uint)(uVar1 >> 1)) * 0x10000) >> 0x10
                )) >> 0x1e & 2 |
          (uint)(*param_1 - ((int)(((uint)*param_3 + (uint)(uVar1 >> 1)) * 0x10000) >> 0x10)) >>
          0x1f;
  uVar2 = (uint)(uVar3 < 3);
  if (uVar3 == 2) {
    uVar2 = 1;
  }
  else if (uVar2 == 0) {
    uVar2 = 3;
    if (uVar3 == 3) {
      DAT_800c3b84 = *param_3;
      DAT_800c3b8a = param_3[3];
      DAT_800c3b86 = param_3[1];
      DAT_800c3b88 = uVar1;
      uVar2 = FUN_8004ed84(param_1,param_2,&DAT_800c3b84);
    }
  }
  else if (uVar3 == 0) {
    DAT_800c3b84 = (param_3[2] + *param_3) - param_3[3];
    DAT_800c3b8a = param_3[3];
    DAT_800c3b86 = param_3[1];
    DAT_800c3b88 = uVar1;
    uVar2 = FUN_8004ed84(param_1,param_2,&DAT_800c3b84);
  }
  return uVar2;
}


