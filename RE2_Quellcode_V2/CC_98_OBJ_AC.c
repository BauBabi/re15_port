/* CC_98_OBJ_AC @ 0x8007b118  (Ghidra headless, raw MIPS overlay) */

void CC_98_OBJ_AC(undefined4 param_1,undefined4 param_2,undefined1 param_3)

{
  undefined4 uVar1;
  int unaff_s0;
  short unaff_s1;
  short unaff_s2;
  
  if (*(char *)(unaff_s0 + 0x1b) == '(') {
    if (*(code **)(&UNK_800ea2d0 + unaff_s2 * 4 + unaff_s1 * 0x40) != (code *)0x0) {
      (**(code **)(&UNK_800ea2d0 + unaff_s2 * 4 + unaff_s1 * 0x40))
                ((int)unaff_s1,(int)unaff_s2,param_3);
    }
  }
  uVar1 = _SsReadDeltaValue((int)unaff_s1,(int)unaff_s2);
  *(undefined4 *)(unaff_s0 + 0x90) = uVar1;
  return;
}


