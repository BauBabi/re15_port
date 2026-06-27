/* StGetNext @ 0x80088d80  (Ghidra headless, raw MIPS overlay) */

u_long StGetNext(u_long **addr,u_long **header)

{
  bool bVar1;
  u_long *puVar2;
  
  puVar2 = (u_long *)(DAT_800ead9c + DAT_800ea234 * 0x20);
  if ((short)*puVar2 == 1) {
    DAT_800ea234 = 0;
    if (DAT_800ead7c != 0) {
      *(short *)puVar2 = 0;
    }
    puVar2 = (u_long *)(DAT_800ead9c + DAT_800ea234 * 0x20);
  }
  bVar1 = (short)*puVar2 != 2;
  if (!bVar1) {
    *(short *)puVar2 = 4;
    *addr = (u_long *)(DAT_800ead9c + DAT_800eae9c * 0x20 + DAT_800ea234 * 0x7e0);
    *header = puVar2;
  }
  return (uint)bVar1;
}


