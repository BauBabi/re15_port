/* DS_vsync_system @ 0x8008b2fc  (Ghidra headless, raw MIPS overlay) */

void DS_vsync_system(void)

{
  undefined1 local_10 [8];
  
  if (0 < DAT_800ad27c) {
    DAT_800ad27c = DAT_800ad27c + -1;
  }
  if (0 < DAT_800ad278) {
    DAT_800ad278 = DAT_800ad278 + -1;
  }
  if (((DAT_800ad254 == 1) || (DAT_800ad254 != 2)) || (DAT_800ad258 == 0xb)) goto code_r0x8008b54c;
  if (DAT_800ad258 == 0xc) {
    if (DAT_800ad300 != 0) {
      local_10[0] = 0;
      if (DAT_800ad260 == 0) {
        DAT_800ad250 = 0x20;
        DS_cw_root(0xe,local_10);
      }
      DAT_800ad300 = 0;
      DSSYS_1_OBJ_6E4();
      return;
    }
    if (DAT_800ad260 == 0) {
      DAT_800ad250 = 0x20;
      DS_cw_root(1,0);
    }
    DAT_800ad300 = 1;
    DSSYS_1_OBJ_6E4();
    return;
  }
  if (DAT_800ad258 == 0xd) {
    DAT_800ad300 = 0;
    DSSYS_1_OBJ_6C4();
    return;
  }
  if (DAT_800ad258 == 0xe) {
    if (DAT_800ad25c != 0x15) {
      if (DAT_800ad25c == 0x16) {
        DAT_800ad270 = DAT_800ad270 + 1;
        if (DAT_800ad260 == 0) {
          DSSYS_1_OBJ_6D0();
          return;
        }
        goto code_r0x8008b54c;
      }
      if (DAT_800ad25c == 0x17) {
        if (DAT_800ad260 == 0) {
          DAT_800ad250 = 0x20;
          DSSYS_1_OBJ_6DC(0x13);
          return;
        }
        goto code_r0x8008b54c;
      }
      if (DAT_800ad25c != 0x18) {
        DSSYS_1_OBJ_6E4();
        return;
      }
    }
  }
  else {
    if (DAT_800ad258 == 0xf) {
      if (DAT_800ad278 != 0) {
        DSSYS_1_OBJ_6E4();
        return;
      }
code_r0x8008b50c:
      DAT_800ad254 = 1;
      DAT_800ad258 = 0xb;
      DSSYS_1_OBJ_6E4();
      return;
    }
    if (DAT_800ad258 == 0x10) {
      if (DAT_800ad26c != '\0') {
        DSSYS_1_OBJ_6A4();
        return;
      }
    }
    else {
      if (DAT_800ad258 != 0x11) goto code_r0x8008b54c;
      if (DAT_800ad26e != '\0') goto code_r0x8008b50c;
    }
  }
  if (DAT_800ad260 == 0) {
    DAT_800ad250 = 0x20;
    DS_cw_root(1,0);
  }
code_r0x8008b54c:
  if ((DAT_800c3f08 != (code *)0x0) && (DAT_800ad238 != 0)) {
    (*DAT_800c3f08)();
  }
  if (((DAT_800ad254 == 1) && (DAT_800ad26f == '\0')) && (DAT_800ad260 == 0)) {
    DAT_800ad250 = 0x21;
    DS_cw_root(1,0);
  }
  return;
}


