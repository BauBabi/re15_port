/* FUN_8004b3b8 @ 0x8004b3b8  (Ghidra headless, raw MIPS overlay) */

void FUN_8004b3b8(int param_1,uint *param_2)

{
  undefined4 *puVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  
  *(uint **)(param_1 + 0x1bc) = param_2;
  if (*param_2 != 0) {
    uVar4 = 0;
    puVar2 = param_2 + 3;
    puVar1 = (undefined4 *)(*(int *)(param_1 + 0x198) + 0x88);
    uVar3 = *(uint *)(*(int *)(param_1 + 0x14) + -4) >> 1;
    do {
      *puVar1 = 0;
      if ((*param_2 & 1 << (uVar4 & 0x1f)) != 0) {
        *puVar1 = puVar2;
        puVar2 = puVar2 + puVar2[2] + 3;
      }
      puVar1 = puVar1 + 0x2b;
      uVar3 = uVar3 - 1;
      uVar4 = uVar4 + 1;
    } while (uVar3 != 0);
  }
  return;
}


