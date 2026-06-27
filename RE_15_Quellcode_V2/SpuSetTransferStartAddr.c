ulong SpuSetTransferStartAddr(ulong addr)

{
  DAT_80076d58 = _spu_FsetRXXa(0xffffffff,addr);
  return addr;
}
