/* SpuStart @ 0x800792ec  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void SpuStart(void)

{
  if (DAT_800ab6e8 == 0) {
    DAT_800ab6e8 = 1;
    FUN_800957a4();
    _SpuDataCallback(_spu_FiDMA);
    DAT_800ab280 = OpenEvent(0xf0000009,0x20,0x2000,0);
    EnableEvent();
    FUN_800957b4();
  }
  return;
}


