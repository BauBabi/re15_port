/* FUN_80034918 @ 0x80034918  (Ghidra headless, raw MIPS overlay) */

void FUN_80034918(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = 1;
  iVar1 = *(int *)(DAT_800cfe14 + 0x198);
  iVar4 = *(int *)(DAT_800ce324 + 0x2c) + DAT_800d4820 * 0x28;
  *(undefined1 *)(iVar4 + 0xf) = 0x80;
  *(undefined1 *)(iVar4 + 0xe) = 0x80;
  *(undefined1 *)(iVar4 + 0xd) = 0x80;
  do {
    iVar3 = 0;
    iVar2 = 0;
    do {
      *(undefined1 *)((short)iVar5 * 3 + iVar4 + 4 + (iVar2 >> 0x10)) = 0;
      iVar3 = iVar3 + 1;
      iVar2 = iVar3 * 0x10000;
    } while (iVar3 * 0x10000 >> 0x10 < 3);
    iVar5 = iVar5 + 1;
  } while (iVar5 * 0x10000 >> 0x10 < 3);
                    /* Possible PsyQ macro: setTile8() */
  *(undefined1 *)(iVar4 + 4) = 0x78;
                    /* Possible PsyQ macro: setTile8() */
  *(undefined1 *)(iVar4 + 5) = 0x78;
  *(undefined1 *)(iVar4 + 6) = 0x78;
  *(undefined2 *)(iVar4 + 0x22) = 4000;
  *(undefined1 *)(iVar4 + 1) = 0;
  *(short *)(iVar4 + 0x10) = (short)*(undefined4 *)(iVar1 + 0x5bc);
  *(short *)(iVar4 + 0x12) = (short)*(undefined4 *)(iVar1 + 0x5c0) + -400;
  *(short *)(iVar4 + 0x14) = (short)*(undefined4 *)(iVar1 + 0x5bc);
  return;
}


