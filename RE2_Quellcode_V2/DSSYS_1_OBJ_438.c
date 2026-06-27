/* DSSYS_1_OBJ_438 @ 0x8008b2a0  (Ghidra headless, raw MIPS overlay) */

undefined4 DSSYS_1_OBJ_438(void)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = CD_cw((undefined1)DAT_800ad23c,DAT_800ad244,0,1);
  if (iVar1 == 0) {
    DAT_800ad264._0_1_ = (undefined1)DAT_800ad23c;
    uVar2 = DSSYS_1_OBJ_47C();
    return uVar2;
  }
  DAT_800ad260 = 0;
  return 0;
}


