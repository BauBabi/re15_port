void StSetRing(u_long *ring_addr,u_long ring_size)

{
  DAT_800bbda4 = ring_addr;
  DAT_800bee74 = ring_size;
  StClearRing();
  return;
}
