/* CC_64_OBJ_78 @ 0x8007afb4  (Ghidra headless, raw MIPS overlay) */

void CC_64_OBJ_78(int param_1)

{
  undefined4 uVar1;
  int unaff_s0;
  short unaff_s2;
  
  uVar1 = _SsReadDeltaValue(param_1 >> 0x10,(int)unaff_s2);
  *(undefined4 *)(unaff_s0 + 0x90) = uVar1;
  return;
}


