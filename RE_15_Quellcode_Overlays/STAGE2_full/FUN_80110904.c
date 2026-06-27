/* FUN_80110904 @ 0x80110904  (Ghidra headless, raw MIPS overlay) */

void FUN_80110904(void)

{
  int iVar1;
  
  iVar1 = func_0x8001af20();
  DAT_8011931c = DAT_8011931c + -0x180 + iVar1 * 3;
  iVar1 = func_0x8001af20();
  DAT_80119320 = DAT_80119320 + -0x200 + iVar1 * 4;
  iVar1 = func_0x8001af20();
  DAT_80119324 = DAT_80119324 + -0x100 + iVar1 * 2;
  return;
}


