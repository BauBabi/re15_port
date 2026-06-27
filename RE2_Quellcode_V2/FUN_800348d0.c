/* FUN_800348d0 @ 0x800348d0  (Ghidra headless, raw MIPS overlay) */

void FUN_800348d0(void)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  do {
    iVar1 = iVar2 << 0x10;
    iVar2 = iVar2 + 1;
    iVar1 = iVar1 >> 0xe;
    *(undefined2 *)((int)&DAT_800dcc54 + iVar1) = 0;
    *(undefined2 *)((int)&DAT_800dcc56 + iVar1) = 0;
  } while (iVar2 * 0x10000 >> 0x10 < 4);
  DAT_800ce338 = &DAT_800dcc50;
  return;
}


