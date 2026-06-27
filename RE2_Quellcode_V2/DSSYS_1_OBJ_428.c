/* DSSYS_1_OBJ_428 @ 0x8008b290  (Ghidra headless, raw MIPS overlay) */

undefined4 DSSYS_1_OBJ_428(void)

{
  undefined1 in_v0;
  int iVar1;
  undefined4 uVar2;
  
  DAT_800ad244 = 0;
  DAT_800ad23c._0_1_ = in_v0;
  iVar1 = CD_cw(in_v0,0,0,1);
  if (iVar1 == 0) {
    DAT_800ad264._0_1_ = (undefined1)DAT_800ad23c;
    uVar2 = DSSYS_1_OBJ_47C();
    return uVar2;
  }
  DAT_800ad260 = 0;
  return 0;
}


