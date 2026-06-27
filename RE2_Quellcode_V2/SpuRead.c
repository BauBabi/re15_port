/* SpuRead @ 0x80085b2c  (Ghidra headless, raw MIPS overlay) */

/* Possible S_W.OBJ/SpuWrite */

ulong SpuRead(uchar *addr,ulong size)

{
  if (0x7eff0 < size) {
    size = 0x7eff0;
  }
  _spu_Fw(addr,size);
  if (DAT_800ab254 == 0) {
    DAT_800ab250 = 0;
  }
  return size;
}


