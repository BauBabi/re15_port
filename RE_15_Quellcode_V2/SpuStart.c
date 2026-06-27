void SpuStart(void)

{
  if (DAT_80077220 == 0) {
    DAT_80077220 = 1;
    FUN_8006e170();
    DAT_80076dc0 = 0;
    _SpuDataCallback(_spu_FiDMA);
    DAT_80076e18 = OpenEvent(0xf0000009,0x20,0x2000,0);
    EnableEvent();
    FUN_8006e468();
  }
  return;
}
