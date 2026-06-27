/* SpuSetNoiseClock @ 0x800835e8  (Ghidra headless, raw MIPS overlay) */

long SpuSetNoiseClock(long n_clock)

{
  long lVar1;
  
  if (n_clock < 0) {
    lVar1 = S_SNC_OBJ_24(n_clock,0);
    return lVar1;
  }
  if (0x3f < n_clock) {
    n_clock = 0x3f;
  }
  *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
       *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) & 0xc0ff |
       (ushort)((n_clock & 0x3fU) << 8);
  return n_clock;
}


