/* DS_cw_root @ 0x8008b1dc  (Ghidra headless, raw MIPS overlay) */

undefined4 DS_cw_root(char param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  CD_flush();
  DAT_800ad23c._0_1_ = param_1;
  if (param_2 != 0) {
    parcpy(0x800ad23d,param_2);
    DAT_800ad244 = 0x800ad23d;
    uVar1 = DSSYS_1_OBJ_3CC(1);
    return uVar1;
  }
  DAT_800ad244 = 0;
  DAT_800ad260 = 1;
  if (param_1 == '\a') {
    if (DAT_800ad26f == '\x01') {
      uVar1 = DSSYS_1_OBJ_428();
      return uVar1;
    }
  }
  else {
    if (param_1 != '\b') {
      uVar1 = DSSYS_1_OBJ_438();
      return uVar1;
    }
    if (DAT_800ad26f != '\x01') {
      DAT_800ad23c._0_1_ = '\x01';
    }
  }
  DAT_800ad244 = 0;
  iVar2 = CD_cw((char)DAT_800ad23c,0,0,1);
  if (iVar2 == 0) {
    DAT_800ad264._0_1_ = (char)DAT_800ad23c;
    uVar1 = DSSYS_1_OBJ_47C();
    return uVar1;
  }
  DAT_800ad260 = 0;
  return 0;
}


