/* FUN_80110988 @ 0x80110988  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110988(void)

{
  int in_v0;
  uint uVar1;
  int in_v1;
  
  if ((in_v1 == in_v0) && (uVar1 = func_0x8001af20(), (uVar1 & 1) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  if (0x1e < *(short *)(_DAT_800ac784 + 0x1dc)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  if ((DAT_800acae7 & 1) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0xb;
  }
  return;
}


