/* DS_sync_for_void @ 0x8008ba60  (Ghidra headless, raw MIPS overlay) */

void DS_sync_for_void(char param_1,byte *param_2)

{
  if ((param_1 == '\x02') && ((char)DAT_800ad23c == '\x0e')) {
    if (((DAT_800ad264._1_1_ ^ DAT_800ad23c._1_1_) & 0x80) != 0) {
      DAT_800ad258 = 0xf;
      DAT_800ad278 = 3;
      DAT_800ad254 = 2;
    }
    DAT_800ad264._1_1_ = DAT_800ad23c._1_1_;
  }
  if (param_1 == '\x05') {
    if ((*param_2 & 0x10) == 0) {
      DAT_800ad254 = 1;
      DAT_800ad258 = 0xb;
    }
    else {
      DAT_800ad254 = 2;
      DAT_800ad258 = 0xc;
      if ((DAT_800c3f0c != (code *)0x0) && (DAT_800ad238 != 0)) {
        (*DAT_800c3f0c)(5);
        DSSYS_1_OBJ_CF8();
        return;
      }
    }
  }
  return;
}


