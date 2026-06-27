/* FUN_80013ac8 @ 0x80013ac8  (Ghidra headless, raw MIPS overlay) */

void FUN_80013ac8(uint param_1)

{
  if (DAT_800d5b5b == '\x01') {
    DAT_800d5b4e = (undefined1)param_1;
    DAT_800d5b4f = 0;
  }
  else {
    DAT_800d5b4e = (undefined1)(param_1 >> 1);
    DAT_800d5b4f = DAT_800d5b4e;
  }
  DAT_800d5b4c = DAT_800d5b4e;
  DAT_800d5b4d = DAT_800d5b4f;
  DsMix((DslATV *)&DAT_800d5b4c);
  return;
}


