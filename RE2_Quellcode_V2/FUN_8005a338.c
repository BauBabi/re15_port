/* FUN_8005a338 @ 0x8005a338  (Ghidra headless, raw MIPS overlay) */

void FUN_8005a338(void)

{
  short sVar1;
  
  if (DAT_800d4c4a != -1) {
    SsVabClose((short)DAT_800d4c4a);
    DAT_800d4c4a = -1;
  }
  DAT_800dbb80 = *(undefined4 *)(DAT_800ce324 + 8);
  DAT_800d75a8 = *(uchar **)(DAT_800ce324 + 0xc);
  DAT_800dbb8c = DAT_800dbb80;
  do {
    sVar1 = SsVabFakeHead(DAT_800d75a8,2,0x14440);
    DAT_800d4c4a = (char)sVar1;
  } while (DAT_800d4c4a == -1);
  do {
    sVar1 = SsVabFakeBody((short)DAT_800d4c4a);
    DAT_800d4c4a = (char)sVar1;
  } while (DAT_800d4c4a == -1);
  return;
}


