/* FUN_8011c024 @ 0x8011c024  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c024(void)

{
  int iStack00000010;
  
  iStack00000010 = _DAT_800ac784 + 0xb0;
  func_0x8001af5c();
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 4;
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  return;
}


