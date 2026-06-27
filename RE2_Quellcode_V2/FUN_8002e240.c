/* FUN_8002e240 @ 0x8002e240  (Ghidra headless, raw MIPS overlay) */

void FUN_8002e240(int param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  int iVar4;
  
  iVar4 = *(int *)(param_1 + 0x14);
  puVar3 = (undefined4 *)(*(int *)(param_1 + 0x10) + 4);
  if (iVar4 != 0) {
    puVar2 = (undefined4 *)(*(int *)(param_1 + 0x10) + 8);
    do {
      iVar4 = iVar4 + -1;
      uVar1 = *puVar3;
      *puVar3 = *puVar2;
      *puVar2 = uVar1;
      puVar2 = puVar2 + 4;
      puVar3 = puVar3 + 4;
    } while (iVar4 != 0);
  }
  return;
}


