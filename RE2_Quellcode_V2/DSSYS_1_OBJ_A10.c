/* DSSYS_1_OBJ_A10 @ 0x8008b878  (Ghidra headless, raw MIPS overlay) */

void DSSYS_1_OBJ_A10(undefined4 param_1,undefined4 param_2,undefined1 param_3,undefined4 param_4)

{
  undefined4 in_v0;
  
  DAT_800ad258 = 0xb;
  DAT_800ad254 = in_v0;
  if ((DAT_800c3f0c != (code *)0x0) && (DAT_800ad238 != 0)) {
    (*DAT_800c3f0c)(param_3,param_4);
  }
  return;
}


