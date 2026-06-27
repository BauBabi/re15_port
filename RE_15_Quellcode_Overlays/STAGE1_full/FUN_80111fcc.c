/* FUN_80111fcc @ 0x80111fcc  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111fcc(short param_1)

{
  char *pcVar1;
  
  pcVar1 = &DAT_801210f8;
  if (param_1 == 0) {
    pcVar1 = &DAT_801210f9;
  }
  else if (0 < param_1) {
    pcVar1 = &DAT_801210fa;
  }
  if (*(char *)(_DAT_800ac784 + 0x94) != *pcVar1) {
    *(char *)(_DAT_800ac784 + 0x94) = *pcVar1;
  }
  return;
}


