/* FUN_8002e288 @ 0x8002e288  (Ghidra headless, raw MIPS overlay) */

void FUN_8002e288(int param_1,int param_2)

{
  undefined2 uVar1;
  uint uVar2;
  uint uVar3;
  undefined2 *puVar4;
  
  uVar2 = *(int *)(param_1 + 0x14) * 2;
  if ((uVar2 & 0xfffe) != 0) {
    puVar4 = (undefined2 *)(param_2 + 0x24);
    uVar2 = uVar2 - 1;
    do {
      uVar1 = puVar4[-6];
      uVar3 = uVar2 & 0xffff;
      puVar4[-6] = *puVar4;
      *puVar4 = uVar1;
      puVar4 = puVar4 + 0x1a;
      uVar2 = uVar2 + 0xffff;
    } while (uVar3 != 0);
  }
  return;
}


