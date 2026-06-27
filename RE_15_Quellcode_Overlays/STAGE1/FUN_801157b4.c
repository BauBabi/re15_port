/* FUN_801157b4 @ 0x801157b4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801157b4(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int in_v0;
  int iVar2;
  uint uVar3;
  undefined2 uVar4;
  int in_v1;
  
  iVar2 = (in_v0 - in_v1 >> 4) + 400;
  uVar4 = (undefined2)iVar2;
  if (iVar2 * 0x10000 >> 0x10 < 0x32) {
    uVar4 = 100;
  }
  *(undefined2 *)(param_3 + 0xbe) = uVar4;
  *(undefined2 *)(param_3 + 0xbc) = uVar4;
  iVar2 = (*(int *)(_DAT_800ac784 + 0x38) - (int)*(short *)(_DAT_800ac784 + 0x1ba) >> 5) + 0x80;
  sVar1 = (short)iVar2;
  if (iVar2 * 0x10000 >> 0x10 < 0x20) {
    sVar1 = 0x20;
  }
  uVar3 = (uint)sVar1;
  uVar3 = uVar3 << 0x10 | uVar3 << 8 | uVar3;
  *(uint *)(param_3 + 0xc4) = uVar3 | *(uint *)(param_3 + 0xc4) & 0xff000000;
  *(uint *)(param_3 + 0xec) = uVar3 | *(uint *)(param_3 + 0xec) & 0xff000000;
  return;
}


