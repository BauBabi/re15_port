/* FUN_80034a04 @ 0x80034a04  (Ghidra headless, raw MIPS overlay) */

void FUN_80034a04(void)

{
  *DAT_800ce550 = 0x17f4;
  DAT_800ce550[1] = 0;
  DAT_800ce550[2] = 0x6db5;
  DAT_800ce550[3] = 0x400;
  *(undefined1 *)((int)DAT_800ce550 + 0xb) = 0;
  *(undefined1 *)(DAT_800ce550 + 5) = 0;
  *(undefined1 *)(DAT_800ce550 + 4) = 6;
  *(undefined1 *)((int)DAT_800ce550 + 9) = 0;
  return;
}


