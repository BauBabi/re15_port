/* FUN_800358f0 @ 0x800358f0  (Ghidra headless, raw MIPS overlay) */

void FUN_800358f0(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  iVar3 = *(int *)(param_2 + 0x48);
  if (DAT_800d7533 != *(char *)(iVar3 + 0x104)) {
    uVar2 = 0;
    param_1 = param_1 * 0x4c;
    DAT_800d7533 = *(char *)(iVar3 + 0x104);
    (&DAT_800d6c4b)[param_1] = (&DAT_800d6c4b)[param_1] + '\x01';
    (&DAT_800d6c49)[param_1] = (&DAT_800d6c4a)[param_1];
    *(ushort *)(&DAT_800d6c4c + param_1) = (*(byte *)(iVar3 + 0x104) & 0xf) << 6;
    iVar1 = 0x38;
    *(ushort *)(&DAT_800d6c4e + param_1) = (ushort)(*(byte *)(iVar3 + 0x104) >> 4) << 8;
    do {
      uVar2 = uVar2 + 1;
                    /* Possible PsyQ macro: setPolyF3() */
      *(undefined2 *)(&DAT_800d6c48 + iVar1 + param_1 + 4) = 0x20;
      *(undefined2 *)(&DAT_800d6c48 + iVar1 + param_1 + 6) = 0x40;
      iVar1 = iVar1 + 8;
    } while (uVar2 < 2);
  }
  return;
}


