/* CC_6_OBJ_3C0 @ 0x8007ac54  (Ghidra headless, raw MIPS overlay) */

void CC_6_OBJ_3C0(int param_1)

{
  undefined4 uVar1;
  int unaff_s0;
  short unaff_s8;
  
  uVar1 = _SsReadDeltaValue(param_1 >> 0x10,(int)unaff_s8);
  *(undefined4 *)(unaff_s0 + 0x90) = uVar1;
  *(undefined1 *)(unaff_s0 + 0x1f) = 0;
  CC_6_OBJ_3F4();
  return;
}


