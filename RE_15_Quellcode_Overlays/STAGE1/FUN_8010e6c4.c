/* FUN_8010e6c4 @ 0x8010e6c4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e6c4(int param_1)

{
  int in_v1;
  
  if (in_v1 != 2) {
    FUN_8010efa4();
    return;
  }
  *(undefined1 *)(param_1 + 5) = 3;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  return;
}


