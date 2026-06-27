/* CC_99_OBJ_F8 @ 0x8007b294  (Ghidra headless, raw MIPS overlay) */

void CC_99_OBJ_F8(int param_1,undefined4 param_2,undefined1 param_3)

{
  undefined4 uVar1;
  short in_t0;
  int unaff_s0;
  
  *(undefined1 *)(unaff_s0 + 0x1b) = param_3;
  *(char *)(unaff_s0 + 0x1f) = *(char *)(unaff_s0 + 0x1f) + '\x01';
  uVar1 = _SsReadDeltaValue(param_1 >> 0x10,(int)in_t0);
  *(undefined4 *)(unaff_s0 + 0x90) = uVar1;
  return;
}


