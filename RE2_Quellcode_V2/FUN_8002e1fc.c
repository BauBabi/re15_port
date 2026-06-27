/* FUN_8002e1fc @ 0x8002e1fc  (Ghidra headless, raw MIPS overlay) */

void FUN_8002e1fc(int param_1,int param_2)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  int iVar3;
  
  iVar3 = *(int *)(param_1 + 0x14) * 2;
  if (iVar3 != 0) {
    puVar2 = (undefined2 *)(param_2 + 0x24);
    do {
      iVar3 = iVar3 + -1;
      uVar1 = puVar2[-6];
      puVar2[-6] = *puVar2;
      *puVar2 = uVar1;
      puVar2 = puVar2 + 0x14;
    } while (iVar3 != 0);
  }
  return;
}


