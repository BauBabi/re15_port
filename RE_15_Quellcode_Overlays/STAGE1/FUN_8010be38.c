/* FUN_8010be38 @ 0x8010be38  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010be38(void)

{
  int in_v0;
  
  *(undefined1 *)(in_v0 + 6) = 0;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 2) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


