void DecDCTReset(int mode)

{
  if (mode == 0) {
    ResetCallback();
  }
  MDEC_reset(mode);
  return;
}
