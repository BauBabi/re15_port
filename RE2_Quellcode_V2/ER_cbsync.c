/* ER_cbsync @ 0x8008cc44  (Ghidra headless, raw MIPS overlay) */

void ER_cbsync(char param_1)

{
  if (param_1 == '\x02') {
    DsReadyCallback(ER_cbready);
  }
  return;
}


