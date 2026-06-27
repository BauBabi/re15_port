/* FUN_80112114 @ 0x80112114  (Ghidra headless overlay RE) */

void FUN_80112114(void)

{
  uint uVar1;
  byte in_v1;
  byte *unaff_s0;
  
  *unaff_s0 = in_v1 | 1;
  uVar1 = func_0x8001af20();
  func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fee5eb0 + (uVar1 & 1)),0x32);
  FUN_80112a48();
  return;
}


