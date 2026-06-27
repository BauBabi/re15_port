/* MIDICC_OBJ_218 @ 0x8007c304  (Ghidra headless, raw MIPS overlay) */

void MIDICC_OBJ_218(void)

{
  undefined4 uVar1;
  int unaff_s0;
  
  uVar1 = _SsReadDeltaValue();
  *(undefined4 *)(unaff_s0 + 0x90) = uVar1;
  return;
}


