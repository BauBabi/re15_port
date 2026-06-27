/* FUN_80086c1c @ 0x80086c1c  (Ghidra headless, raw MIPS overlay) */

/* Possible C_003.OBJ/StUnSetRing */

void FUN_80086c1c(void)

{
  FUN_800957a4();
  if (DAT_800ace10 == 1) {
    DsDataCallback((func *)0x0);
    DsReadyCallback((DslCB)0x0);
  }
  else {
    CdDataCallback((func *)0x0);
    CdReadyCallback((CdlCB)0x0);
  }
  *PTR_CDROM_REG0_800acd54 = 0;
  *PTR_CDROM_REG3_800acd60 = 0;
  FUN_800957b4();
  return;
}


