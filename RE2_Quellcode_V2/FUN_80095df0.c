/* FUN_80095df0 @ 0x80095df0  (Ghidra headless, raw MIPS overlay) */

uint FUN_80095df0(undefined4 param_1,int param_2,int param_3)

{
  int *piVar1;
  uint uVar2;
  
  piVar1 = (int *)(*DAT_800c39b0)();
  if (param_2 == 3) {
    uVar2 = (uint)*(byte *)(piVar1 + 0x39);
  }
  else if (param_2 < 4) {
    if (param_2 == 1) {
      uVar2 = (uint)*(byte *)(piVar1 + 0x3a);
    }
    else {
      uVar2 = 0;
      if (param_2 == 2) {
        uVar2 = (uint)*(ushort *)((int)piVar1 + 0xe6);
      }
    }
  }
  else if (param_2 == 4) {
    if (param_3 < 0) {
      uVar2 = (uint)*(byte *)((int)piVar1 + 0xe3);
    }
    else if (param_3 < (int)(uint)*(byte *)((int)piVar1 + 0xe3)) {
      uVar2 = (uint)*(ushort *)(param_3 * 2 + *piVar1);
    }
    else {
      uVar2 = 0;
    }
  }
  else {
    uVar2 = 0;
    if (param_2 == 100) {
      uVar2 = piVar1[0x13];
    }
  }
  return uVar2;
}


