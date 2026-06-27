/* DsReadMode @ 0x8008c730  (Ghidra headless, raw MIPS overlay) */

void DsReadMode(uint param_1)

{
  if (param_1 < 2) {
    DAT_800ad310 = param_1;
  }
  return;
}


