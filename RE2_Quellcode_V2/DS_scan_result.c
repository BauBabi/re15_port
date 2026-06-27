/* DS_scan_result @ 0x8008bc3c  (Ghidra headless, raw MIPS overlay) */

void DS_scan_result(byte *param_1)

{
  DAT_800ad26c = *param_1 >> 7;
  DAT_800ad26d = *param_1 >> 6 & 1;
  DAT_800ad26e = *param_1 >> 5 & 1;
  DAT_800ad26f = *param_1 >> 1 & 1;
  rescpy(&DAT_800ad248);
  return;
}


