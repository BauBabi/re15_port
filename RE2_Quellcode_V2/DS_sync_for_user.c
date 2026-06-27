/* DS_sync_for_user @ 0x8008b6e0  (Ghidra headless, raw MIPS overlay) */

void DS_sync_for_user(char param_1,byte *param_2)

{
  uint uVar1;
  
  if (param_1 == '\x02') {
    uVar1 = DAT_800ad23c & 0xff;
    if (uVar1 == 0xe) {
      if (((DAT_800ad264._1_1_ ^ DAT_800ad23c._1_1_) & 0x80) != 0) {
        DAT_800ad258 = 0xf;
        DAT_800ad254 = 2;
        DAT_800ad278 = 3;
        DSSYS_1_OBJ_90C();
        return;
      }
      DAT_800ad254 = 1;
      DAT_800ad258 = 0xb;
      DAT_800ad264._0_2_ = CONCAT11(DAT_800ad23c._1_1_,(undefined1)DAT_800ad264);
      DSSYS_1_OBJ_A24();
      return;
    }
    if (uVar1 == 3) {
      DSSYS_1_OBJ_948();
      return;
    }
    if ((uVar1 == 6) || (uVar1 == 0x1b)) {
      DAT_800ad254 = 2;
      DAT_800ad258 = 0x11;
      DAT_800ad268 = CONCAT13((undefined1)DAT_800ad23c,(undefined3)DAT_800ad268);
      DSSYS_1_OBJ_A24();
      return;
    }
    switch(uVar1) {
    case 2:
      DAT_800ad268 = DAT_800ad268 & 0xffff0000 | (DAT_800ad240 << 0x18 | DAT_800ad23c >> 8) >> 0x10;
      DAT_800ad264 = DAT_800ad264 & 0xffff | (DAT_800ad23c >> 8) << 0x10;
      DSSYS_1_OBJ_A10();
      return;
    case 3:
    case 6:
      DAT_800ad268 = CONCAT13((undefined1)DAT_800ad23c,(undefined3)DAT_800ad268);
      DSSYS_1_OBJ_A10();
      return;
    case 0x15:
    case 0x16:
      DAT_800ad268._0_3_ = CONCAT12((undefined1)DAT_800ad23c,(undefined2)DAT_800ad268);
      DSSYS_1_OBJ_A10();
      return;
    }
  }
  else if ((*param_2 & 0x10) != 0) {
    DAT_800ad254 = 2;
    DSSYS_1_OBJ_A1C();
    return;
  }
  DAT_800ad254 = 1;
  DAT_800ad258 = 0xb;
  if ((DAT_800c3f0c != (code *)0x0) && (DAT_800ad238 != 0)) {
    (*DAT_800c3f0c)(param_1,param_2);
  }
  return;
}


