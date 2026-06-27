/* FUN_8010c03c @ 0x8010c03c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c03c(void)

{
  uint *in_v1;
  uint unaff_s0;
  
  *in_v1 = *in_v1 & unaff_s0;
  func_0x80012a0c(5000);
  if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
    *(short *)(_DAT_800ac784 + 0x1dc) = *(short *)(_DAT_800ac784 + 0x1dc) + -1;
  }
  if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)(_DAT_800ac784 + 0x1de) = *(short *)(_DAT_800ac784 + 0x1de) + 1;
    FUN_8010c8ac();
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0;
  return;
}


