/* FUN_80110104 @ 0x80110104  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110104(void)

{
  int iVar1;
  
  iVar1 = func_0x8001af20();
  _DAT_8011931c = _DAT_8011931c + -0x180 + iVar1 * 3;
  iVar1 = func_0x8001af20();
  _DAT_80119320 = _DAT_80119320 + -0x200 + iVar1 * 4;
  iVar1 = func_0x8001af20();
  _DAT_80119324 = _DAT_80119324 + -0x100 + iVar1 * 2;
  return;
}


