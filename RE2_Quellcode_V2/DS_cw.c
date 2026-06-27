/* DS_cw @ 0x8008b058  (Ghidra headless, raw MIPS overlay) */

undefined4 DS_cw(undefined1 param_1)

{
  undefined4 uVar1;
  
  if (((DAT_800ad260 == 0) && (DAT_800ad238 != 0)) && (DAT_800ad254 == 1)) {
    DAT_800ad250 = 0x1f;
    DAT_800ad254 = 2;
    DAT_800ad258 = 0xb;
    DS_cw_root(param_1);
    uVar1 = DSSYS_1_OBJ_264();
    return uVar1;
  }
  return 0;
}


