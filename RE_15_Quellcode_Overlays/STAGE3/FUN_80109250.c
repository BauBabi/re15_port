/* FUN_80109250 @ 0x80109250  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109250(int param_1)

{
  int in_v0;
  undefined2 in_v1;
  
  *(undefined2 *)(in_v0 + 0x9c) = in_v1;
  if (param_1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  *(short *)(_DAT_800ac784 + 0x166) = *(short *)(_DAT_800ac784 + 0x166) + -2;
  if (*(short *)(_DAT_800ac784 + 0x166) < 100) {
    *(undefined2 *)(_DAT_800ac784 + 0x166) = 100;
    FUN_80109e64();
    return;
  }
  return;
}


