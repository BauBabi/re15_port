/* FUN_80112f34 @ 0x80112f34  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112f34(void)

{
  uint uVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  FUN_801118a8();
  if (*(char *)(_DAT_800ac784 + 9) == '\x03') {
    uVar1 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = (*(int *)(_DAT_800ac784 + 0x38) + -100) - (uVar1 & 0xf);
    if (*(int *)(_DAT_800ac784 + 0x38) < -9000) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
  }
  if (*(char *)(_DAT_800ac784 + 9) == '\x04') {
    uVar1 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 100 + (uVar1 & 0xf);
    if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
  }
  return;
}


