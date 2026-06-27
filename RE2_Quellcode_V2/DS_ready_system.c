/* DS_ready_system @ 0x8008bb70  (Ghidra headless, raw MIPS overlay) */

void DS_ready_system(char param_1,byte *param_2)

{
  if ((*(int *)(&DAT_800ad280 + (uint)(byte)DAT_800ad23c * 4) != 0) || (param_1 == '\x05')) {
    DS_scan_result(param_2);
  }
  if ((*param_2 & 0x10) != 0) {
    DAT_800ad254 = 2;
    DAT_800ad258 = 0xc;
  }
  if ((DAT_800c3f10 != (code *)0x0) && (DAT_800ad238 != 0)) {
    (*DAT_800c3f10)(param_1,param_2);
  }
  return;
}


