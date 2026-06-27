/* DS_read_cbready @ 0x8008c374  (Ghidra headless, raw MIPS overlay) */

void DS_read_cbready(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  DAT_800ad320 = VSync(-1);
  if ((DAT_800ad310 & 1) == 0) {
    if (0 < DAT_800ad30c) {
      DsGetSector(DAT_800ad308,DAT_800ad304);
      DAT_800ad308 = (void *)((int)DAT_800ad308 + DAT_800ad304 * 4);
      DAT_800ad30c = DAT_800ad30c + -1;
    }
    iVar1 = VSync(-1);
    if (DAT_800ad31c + 0x4b0 < iVar1) {
      DAT_800ad30c = -1;
    }
    if ((DAT_800ad30c == 0) || (iVar1 = VSync(-1), DAT_800ad31c + 0x4b0 < iVar1)) {
      DsReadBreak();
      if (DAT_800ad328 != (code *)0x0) {
        uVar2 = 2;
        if (DAT_800ad30c < 0) {
          uVar2 = 5;
        }
        (*DAT_800ad328)(uVar2,param_2);
      }
    }
  }
  else {
    if (0 < DAT_800ad30c) {
      DsGetSector2(DAT_800ad308,DAT_800ad304);
      DAT_800ad314 = param_2;
      DSREAD_OBJ_2EC();
      return;
    }
    DsReadBreak();
    if (DAT_800ad328 != (code *)0x0) {
      DSREAD_OBJ_2D0();
      return;
    }
  }
  return;
}


