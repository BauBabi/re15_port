/* FUN_800460ac @ 0x800460ac  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_800460ac(int param_1)

{
  int iVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  
  iVar4 = 6;
  puVar3 = (undefined4 *)(*(int *)(param_1 + 0x198) + 0xd70);
  if (*(char *)(param_1 + 8) == '\x0e') {
    puVar3 = (undefined4 *)(*(int *)(param_1 + 0x198) + 0xb6c);
  }
  piVar2 = puVar3 + 0x25;
  do {
    *(undefined1 *)(piVar2 + 1) = 0xff;
    *puVar3 = 0;
    piVar2[6] = 0;
    piVar2[-0x1f] = 0x1000;
    piVar2[-0x1e] = 0;
    piVar2[-0x1d] = 0x1000;
    piVar2[-0x1c] = 0;
    piVar2[-0x1b] = 0x1000;
    FUN_8002ce94(piVar2[-8],puVar3 + 6,puVar3 + 0x12);
    puVar3 = puVar3 + 0x56;
    *(undefined2 *)((int)piVar2 + -0x16) = *(undefined2 *)(param_1 + 0x76);
    iVar1 = *(int *)(param_1 + 0x198);
    iVar4 = iVar4 + -1;
    piVar2[-8] = iVar1 + 0x7ac;
    *piVar2 = iVar1 + 0x764;
    piVar2 = piVar2 + 0x56;
  } while (iVar4 != 0);
  *(undefined2 *)(*(int *)(param_1 + 0x198) + 0x7e8) = 0;
  return 0;
}


