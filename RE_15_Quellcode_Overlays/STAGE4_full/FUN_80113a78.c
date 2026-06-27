/* FUN_80113a78 @ 0x80113a78  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113a78(void)

{
  uint uVar1;
  
  if (*(char *)(_DAT_800ac784 + 9) == '\b') {
    uVar1 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = (*(int *)(_DAT_800ac784 + 0x38) + -0x3c) - (uVar1 & 0xf);
    if (*(int *)(_DAT_800ac784 + 0x38) < -0xe10) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  if (*(char *)(_DAT_800ac784 + 9) == '\t') {
    uVar1 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x3c + (uVar1 & 0xf);
    if (-0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
  }
  return;
}


