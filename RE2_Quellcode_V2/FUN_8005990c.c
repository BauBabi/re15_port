/* FUN_8005990c @ 0x8005990c  (Ghidra headless, raw MIPS overlay) */

void FUN_8005990c(char param_1)

{
  if (param_1 != '\0') {
    SsStart();
  }
  SsSetMVol(DAT_800dcb9c,DAT_800dcb9e);
  SsSetRVol((short)DAT_800d765c,DAT_800d765c._2_2_);
  SsSetSerialAttr('\0','\0','\0');
  SsSetSerialVol('\0',(short)DAT_800d4c78,DAT_800d4c78._2_2_);
  return;
}


