u_long StGetNext(u_long **addr,u_long **header)

{
  bool bVar1;
  u_long *puVar2;
  
  puVar2 = (u_long *)(DAT_800bb4cc * 0x20 + DAT_800bbda4);
  if ((short)*puVar2 == 1) {
    DAT_800bb4cc = 0;
    if (DAT_800bbd94 != 0) {
      *(short *)puVar2 = 0;
    }
    puVar2 = (u_long *)(DAT_800bb4cc * 0x20 + DAT_800bbda4);
  }
  bVar1 = (short)*puVar2 != 2;
  if (!bVar1) {
    *(short *)puVar2 = 4;
    *addr = (u_long *)(DAT_800bee74 * 0x20 + DAT_800bbda4 + DAT_800bb4cc * 0x7e0);
    *header = puVar2;
  }
  return (uint)bVar1;
}
