/* StSetRing @ 0x80086b8c  (Ghidra headless, raw MIPS overlay) */

void StSetRing(u_long *ring_addr,u_long ring_size)

{
  DAT_800ead9c = ring_addr;
  DAT_800eae9c = ring_size;
  StClearRing();
  return;
}


