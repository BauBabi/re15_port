/* FUN_80110aac @ 0x80110aac  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110aac(short param_1)

{
  char *pcVar1;
  
  pcVar1 = &DAT_8011ec10;
  if (param_1 == 0) {
    pcVar1 = &DAT_8011ec11;
  }
  else if (0 < param_1) {
    pcVar1 = &DAT_8011ec12;
  }
  if (*(char *)(_DAT_800ac784 + 0x94) != *pcVar1) {
    *(char *)(_DAT_800ac784 + 0x94) = *pcVar1;
  }
  return;
}


