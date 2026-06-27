/* FUN_8002e1b4 @ 0x8002e1b4  (Ghidra headless, raw MIPS overlay) */

void FUN_8002e1b4(int param_1)

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
      puVar2 = puVar2 + 3;
      puVar3 = puVar3 + 3;
    } while (iVar4 != 0);
  }
  return;
}


