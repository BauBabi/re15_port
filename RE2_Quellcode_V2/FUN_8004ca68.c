/* FUN_8004ca68 @ 0x8004ca68  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8004ca68(int param_1,undefined4 param_2,uint param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  
  iVar3 = *(int *)(param_1 + 0x8c);
  uVar2 = (param_3 ^ *(int *)(param_1 + 0x84) - (int)*(short *)(param_1 + 0x118)) >> 0x1f |
          (param_4 ^ iVar3 - *(short *)(param_1 + 0x11a)) >> 0x1e & 2;
  if (uVar2 != 2) {
    if (uVar2 < 3) {
      if (uVar2 == 1) {
        uVar2 = param_3;
        if ((int)param_3 < 0) {
          uVar2 = -param_3;
        }
        iVar3 = *(int *)(param_1 + 0x84) + param_3;
        if ((int)uVar2 < 0x191) goto LAB_8004caf0;
        goto LAB_8004cb14;
      }
    }
    else if (uVar2 == 3) goto LAB_8004cb14;
    uVar2 = param_3;
    if ((int)param_3 < 0) {
      uVar2 = -param_3;
    }
    uVar1 = param_4;
    if ((int)param_4 < 0) {
      uVar1 = -param_4;
    }
    if ((int)uVar1 <= (int)uVar2) {
      iVar3 = *(int *)(param_1 + 0x8c);
LAB_8004cba8:
      *(uint *)(param_1 + 0x8c) = iVar3 + param_4;
      return 1;
    }
    iVar3 = *(int *)(param_1 + 0x84) + param_3;
LAB_8004caf0:
    *(int *)(param_1 + 0x84) = iVar3;
    return 1;
  }
  uVar2 = param_4;
  if ((int)param_4 < 0) {
    uVar2 = -param_4;
  }
  if ((int)uVar2 < 0x191) goto LAB_8004cba8;
LAB_8004cb14:
  uVar2 = param_3;
  if ((int)param_3 < 0) {
    uVar2 = -param_3;
  }
  uVar1 = param_4;
  if ((int)param_4 < 0) {
    uVar1 = -param_4;
  }
  if ((int)uVar2 < (int)uVar1) {
    if ((int)uVar2 < 0x321) {
      iVar3 = *(int *)(param_1 + 0x84) + param_3;
      goto LAB_8004caf0;
    }
  }
  else if ((int)uVar1 < 0x321) {
    *(uint *)(param_1 + 0x8c) = *(int *)(param_1 + 0x8c) + param_4;
    return 1;
  }
  *(int *)(param_1 + 0x84) = (int)*(short *)(param_1 + 0x118);
  *(int *)(param_1 + 0x8c) = (int)*(short *)(param_1 + 0x11a);
  return 1;
}


