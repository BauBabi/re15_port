/* DSSYS_1_OBJ_3CC @ 0x8008b234  (Ghidra headless, raw MIPS overlay) */

undefined4 DSSYS_1_OBJ_3CC(uint param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  DAT_800ad260 = param_1;
  if ((char)DAT_800ad23c == '\a') {
    if (DAT_800ad26f == param_1) {
      uVar1 = DSSYS_1_OBJ_428();
      return uVar1;
    }
  }
  else {
    if ((char)DAT_800ad23c != '\b') {
      uVar1 = DSSYS_1_OBJ_438();
      return uVar1;
    }
    if (DAT_800ad26f != param_1) {
      DAT_800ad23c._0_1_ = '\x01';
      DAT_800ad244 = 0;
    }
  }
  iVar2 = CD_cw((char)DAT_800ad23c,DAT_800ad244,0,1);
  if (iVar2 == 0) {
    DAT_800ad264._0_1_ = (char)DAT_800ad23c;
    uVar1 = DSSYS_1_OBJ_47C();
    return uVar1;
  }
  DAT_800ad260 = 0;
  return 0;
}


