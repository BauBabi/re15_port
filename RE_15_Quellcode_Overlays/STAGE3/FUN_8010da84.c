/* FUN_8010da84 @ 0x8010da84  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010da84(void)

{
  uint uVar1;
  
  uVar1 = func_0x8001af20();
  if ((uVar1 & 1) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  return;
}


