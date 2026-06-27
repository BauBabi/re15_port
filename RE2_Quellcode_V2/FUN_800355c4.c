/* FUN_800355c4 @ 0x800355c4  (Ghidra headless, raw MIPS overlay) */

void FUN_800355c4(uint *param_1)

{
  int iVar1;
  uint *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  undefined2 uVar5;
  uint uVar6;
  
  FUN_80035408(param_1,param_1 + 0xe);
  uVar4 = 0x8000;
  *(undefined1 *)((int)param_1 + 0xd) = 0xff;
  puVar3 = &DAT_800cfe14;
  uVar6 = 0;
  uVar5 = 0;
  if (DAT_800ce334 != &DAT_800cfe14) {
    do {
      puVar2 = (uint *)*puVar3;
      if (((param_1 != puVar2) && ((*puVar2 & 1) != 0)) &&
         (iVar1 = FUN_80034d0c(puVar2,param_1), iVar1 != 0)) {
        uVar6 = uVar6 | uVar4;
      }
      uVar5 = (undefined2)uVar6;
      puVar3 = puVar3 + 1;
      uVar4 = uVar4 >> 1;
    } while (puVar3 != DAT_800ce334);
  }
  *(undefined2 *)((int)param_1 + 0xe) = uVar5;
  return;
}


