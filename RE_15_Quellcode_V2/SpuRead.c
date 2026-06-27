ulong SpuRead(uchar *addr,ulong size)

{
  if (0x7f000 < size) {
    size = 0x7f000;
  }
  _spu_write(addr,size);
  if (DAT_80076d90 == 0) {
    DAT_80076d8c = 0;
  }
  return size;
}
