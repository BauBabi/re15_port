/* DS_read_cbsync @ 0x8008c340  (Ghidra headless, raw MIPS overlay) */

void DS_read_cbsync(char param_1)

{
  if (param_1 == '\x02') {
    DsStartReadySystem(DS_read_cbready,-1);
  }
  return;
}


