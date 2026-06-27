/* FUN_80100a28 @ 0x80100a28  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100a28(void)

{
  int iVar1;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined2 *)(iVar1 + 0x3e6) = 0x1000;
  *(uint *)(iVar1 + 0x35c) = *(uint *)(iVar1 + 0x35c) | 0x400;
  *(undefined2 *)(iVar1 + 0x3e4) = 0x7e8;
  *(undefined2 *)(iVar1 + 1000) = 0x7e8;
  *(undefined4 *)(iVar1 + 0x3f0) = 0x80;
  *(undefined4 *)(iVar1 + 0x3f4) = 0x8080e0;
  *(uint *)(iVar1 + 0x35c) = *(uint *)(iVar1 + 0x35c) | 0x80;
  return;
}


