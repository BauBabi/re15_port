/* FUN_800389e8 @ 0x800389e8  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_800389e8(undefined4 param_1,uint *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  
  if (((*param_2 & 2) == 0) && (puVar2 = &DAT_800cfe14, DAT_800ce334 != &DAT_800cfe14)) {
    do {
      if ((param_2 != (uint *)*puVar2) &&
         (((*(uint *)*puVar2 & 1) != 0 && (iVar1 = FUN_800352d8(param_1,param_2), iVar1 != 0)))) {
        return 1;
      }
      puVar2 = puVar2 + 1;
    } while (puVar2 != DAT_800ce334);
  }
  return 0;
}


