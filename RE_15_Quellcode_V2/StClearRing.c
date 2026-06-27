void StClearRing(void)

{
  DAT_800bb4cc = 0;
  DAT_800bb4c8 = 0;
  DAT_800bb4c4 = 0;
  DAT_800b5220 = 0;
  init_ring_status(0,DAT_800bee74);
  DAT_800b24fc = 0;
  DAT_800b2208 = 0;
  DAT_800aa5e0 = 0;
  return;
}
