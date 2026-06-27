/* DSSYS_1_OBJ_A0C @ 0x8008b874  (Ghidra headless, raw MIPS overlay) */

void DSSYS_1_OBJ_A0C(undefined4 param_1,undefined4 param_2,undefined1 param_3,undefined4 param_4)

{
  DAT_800ad254 = 1;
  DAT_800ad258 = 0xb;
  if ((DAT_800c3f0c != (code *)0x0) && (DAT_800ad238 != 0)) {
    (*DAT_800c3f0c)(param_3,param_4);
  }
  return;
}


