/* DS_sync_system @ 0x8008b5ec  (Ghidra headless, raw MIPS overlay) */

void DS_sync_system(char param_1,byte *param_2)

{
  if ((*(int *)(&DAT_800ad280 + (uint)(byte)DAT_800ad23c * 4) != 0) || (param_1 == '\x05')) {
    DS_scan_result(param_2);
  }
  DAT_800ad260 = 0;
  if ((*param_2 & 0x10) != 0) {
    param_1 = '\x05';
  }
  if (DAT_800ad250 != 0x1f) {
    if (DAT_800ad250 != 0x20) {
      DSSYS_1_OBJ_83C();
      return;
    }
    DS_sync_for_system(param_1,param_2);
    DSSYS_1_OBJ_844();
    return;
  }
  DS_sync_for_user(param_1,param_2);
  DSSYS_1_OBJ_844();
  return;
}


