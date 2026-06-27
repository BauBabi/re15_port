void CD_flush(void)

{
  if ((CDROM_REG3 & 7) != 0) {
    do {
    } while( true );
  }
  DAT_80078b82 = 0;
  DAT_80078b81 = 0;
  DAT_80078b80 = 2;
  CDROM_REG0 = 0;
  CDROM_REG3 = 0;
  COMMON_DELAY = 0x1325;
  return;
}
