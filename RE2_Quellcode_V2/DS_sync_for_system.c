/* DS_sync_for_system @ 0x8008b8d8  (Ghidra headless, raw MIPS overlay) */

void DS_sync_for_system(char param_1,byte *param_2)

{
  undefined4 uVar1;
  
  if (DAT_800ad258 == 0xc) {
    DAT_800ad258 = 0xd;
  }
  if (param_1 == '\x02') {
    if (DAT_800ad258 == 0xd) {
      if ((*param_2 & 0x10) == 0) {
        DAT_800ad254 = 2;
        DAT_800ad258 = 0xe;
        DAT_800ad25c = 0x15;
        DAT_800ad274 = DAT_800ad274 + 1;
        DSSYS_1_OBJ_BF0();
        return;
      }
    }
    else if (DAT_800ad258 == 0xe) {
      if (DAT_800ad25c == 0x15) {
        if ((*param_2 & 0x10) == 0) {
          DAT_800ad254 = 2;
          DAT_800ad25c = 0x16;
          DAT_800ad270 = 0;
          DSSYS_1_OBJ_BF0();
          return;
        }
      }
      else if (DAT_800ad25c == 0x16) {
        uVar1 = 0x17;
        if ((*param_2 & 2) != 0) {
DSSYS_1_OBJ_B90:
          DAT_800ad254 = 2;
          DAT_800ad25c = uVar1;
          DSSYS_1_OBJ_BF0();
          return;
        }
        if (300 < DAT_800ad270) {
          DAT_800ad254 = 3;
          DSSYS_1_OBJ_BF0();
          return;
        }
      }
      else {
        uVar1 = 0x18;
        if (DAT_800ad25c == 0x17) goto DSSYS_1_OBJ_B90;
        if ((DAT_800ad25c == 0x18) && (*param_2 == 2)) {
          DAT_800ad254 = 1;
          DAT_800ad258 = 0xb;
          DSSYS_1_OBJ_BF0();
          return;
        }
      }
    }
  }
  else if ((*param_2 & 0x10) != 0) {
    DAT_800ad254 = 2;
    DAT_800ad258 = 0xc;
  }
  return;
}


