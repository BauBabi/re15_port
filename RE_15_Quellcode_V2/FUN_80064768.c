void FUN_80064768(void)

{
  FUN_8006e170();
  CdDataCallback((func *)0x0);
  CdReadyCallback((CdlCB)0x0);
  CDROM_REG0 = 0;
  CDROM_REG3 = 0;
  FUN_8006e468();
  return;
}
