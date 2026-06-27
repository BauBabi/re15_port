/* FUN_8002c94c @ 0x8002c94c  (Ghidra headless, raw MIPS overlay) */

void FUN_8002c94c(void)

{
  FUN_800957a4();
  DAT_800d7698 = OpenEvent(0xf4000001,4,0x2000,0);
  DAT_800d7828 = OpenEvent(0xf4000001,0x100,0x2000,0);
  DAT_800d782c = OpenEvent(0xf4000001,0x2000,0x2000,0);
  DAT_800d7830 = OpenEvent(0xf4000001,0x8000,0x2000,0);
  DAT_800d8cd4 = OpenEvent(0xf0000011,4,0x2000,0);
  DAT_800d8cdc = OpenEvent(0xf0000011,0x100,0x2000,0);
  DAT_800d8ce0 = OpenEvent(0xf0000011,0x8000,0x2000,0);
  FUN_800957b4();
  EnableEvent(DAT_800d7698);
  EnableEvent(DAT_800d7828);
  EnableEvent(DAT_800d782c);
  EnableEvent(DAT_800d7830);
  EnableEvent(DAT_800d8cd4);
  EnableEvent(DAT_800d8cdc);
  EnableEvent(DAT_800d8ce0);
  return;
}


