/* MIDIMETA_OBJ_1A0 @ 0x8007c4c4  (Ghidra headless, raw MIPS overlay) */

void MIDIMETA_OBJ_1A0(short param_1,short param_2)

{
  undefined2 in_v0;
  undefined4 uVar1;
  int unaff_s0;
  
  *(undefined2 *)(unaff_s0 + 0x54) = in_v0;
  uVar1 = _SsReadDeltaValue((int)param_1,(int)param_2);
  *(undefined4 *)(unaff_s0 + 0x90) = uVar1;
  return;
}


