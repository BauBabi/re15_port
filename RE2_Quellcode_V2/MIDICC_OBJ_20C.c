/* MIDICC_OBJ_20C @ 0x8007c2f8  (Ghidra headless, raw MIPS overlay) */

void MIDICC_OBJ_20C(int param_1,short param_2)

{
  undefined4 uVar1;
  int unaff_s0;
  
  uVar1 = _SsReadDeltaValue(param_1 >> 0x10,(int)param_2);
  *(undefined4 *)(unaff_s0 + 0x90) = uVar1;
  return;
}


