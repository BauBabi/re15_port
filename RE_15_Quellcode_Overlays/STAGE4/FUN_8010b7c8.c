/* FUN_8010b7c8 @ 0x8010b7c8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b7c8(void)

{
  uint *extraout_v1;
  uint unaff_s0;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee649c))();
    return;
  }
  FUN_80107c64();
  if ((char)extraout_v1[0x78] != '\0') {
    *extraout_v1 = *extraout_v1 & unaff_s0;
    func_0x80012a0c(5000);
  }
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


