/* FUN_801006ec @ 0x801006ec  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801006ec(void)

{
  int iVar1;
  
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined2 *)(iVar1 + 0x89a) = 0x1000;
  *(uint *)(iVar1 + 0x810) = *(uint *)(iVar1 + 0x810) | 0x400;
  *(undefined2 *)(iVar1 + 0x898) = 0x5f4;
  *(undefined2 *)(iVar1 + 0x89c) = 0x5f4;
  *(undefined4 *)(iVar1 + 0x8a4) = 0x80;
  *(undefined4 *)(iVar1 + 0x8a8) = 0x8080e0;
  *(uint *)(iVar1 + 0x810) = *(uint *)(iVar1 + 0x810) | 0x80;
  return;
}


