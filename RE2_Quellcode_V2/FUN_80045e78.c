/* FUN_80045e78 @ 0x80045e78  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80045e78(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined4 *puVar4;
  uint *puVar5;
  short sVar6;
  
  iVar2 = 6;
  iVar1 = *(int *)(param_1 + 0x198);
  puVar4 = (undefined4 *)(iVar1 + 0xd70);
  if (*(char *)(param_1 + 8) == '\x0e') {
    puVar4 = (undefined4 *)(iVar1 + 0xb6c);
  }
  sVar6 = 0;
  piVar3 = puVar4 + 0x25;
  do {
    if ((char)piVar3[1] == -1) {
      *(undefined1 *)(piVar3 + 1) = 0;
      *puVar4 = 1;
      piVar3[-0x1a] = 200;
      piVar3[6] = 1;
      piVar3[-0x19] = 0x390;
      piVar3[-0x18] = 0;
      if (param_2 == 1) {
        piVar3[-0x18] = 100;
      }
      if (param_2 == 2) {
        piVar3[-0x18] = -100;
      }
      piVar3[-0x1f] = 0x1000;
      piVar3[-0x1e] = 0;
      piVar3[-0x1d] = 0x1000;
      piVar3[-0x1c] = 0;
      piVar3[-0x1b] = 0x1000;
      FUN_8002ce94(piVar3[-8],puVar4 + 6,puVar4 + 0x12);
      *(undefined2 *)((int)piVar3 + -0x16) = *(undefined2 *)(param_1 + 0x76);
      iVar2 = *(int *)(param_1 + 0x198);
      *puVar4 = 1;
      piVar3[-8] = iVar2 + 0x7ac;
      *piVar3 = iVar2 + 0x764;
      *(short *)(iVar1 + 0x7e8) = sVar6;
      return 1;
    }
    piVar3 = piVar3 + 0x56;
    puVar4 = puVar4 + 0x56;
    iVar2 = iVar2 + -1;
    sVar6 = sVar6 + 1;
  } while (iVar2 != 0);
  sVar6 = *(short *)(iVar1 + 0x7e8) + 1;
  *(short *)(iVar1 + 0x7e8) = sVar6;
  if (2 < sVar6) {
    *(undefined2 *)(iVar1 + 0x7e8) = 0;
  }
  puVar5 = (uint *)(*(int *)(param_1 + 0x198) + *(short *)(iVar1 + 0x7e8) * 0x158 + 0xd70);
  if (*(char *)(param_1 + 8) == '\x0e') {
    puVar5 = puVar5 + -0x81;
  }
  *(undefined1 *)(puVar5 + 0x26) = 0;
  *puVar5 = 1;
  puVar5[0x2b] = 1;
  *(undefined2 *)((int)puVar5 + 0x7e) = *(undefined2 *)(param_1 + 0x76);
  iVar1 = *(int *)(param_1 + 0x198);
  puVar5[6] = 0x1000;
  puVar5[8] = 0x1000;
  puVar5[10] = 0x1000;
  puVar5[0xb] = 200;
  *puVar5 = 1;
  puVar5[0xc] = 0x390;
  puVar5[7] = 0;
  puVar5[9] = 0;
  puVar5[0xd] = 0;
  puVar5[0x1d] = iVar1 + 0x7ac;
  puVar5[0x25] = iVar1 + 0x764;
  *puVar5 = *puVar5 | 0x40;
  if (param_2 == 1) {
    puVar5[0xd] = 100;
  }
  if (param_2 == 2) {
    puVar5[0xd] = 0xffffff9c;
  }
  FUN_8002ce94(puVar5[0x1d],puVar5 + 6,puVar5 + 0x12);
  return 0;
}


