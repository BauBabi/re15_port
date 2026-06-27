/* SpuWritePartly @ 0x80085d44  (Ghidra headless, raw MIPS overlay) */

ulong SpuWritePartly(uchar *addr,ulong size)

{
  uint uVar1;
  uint uVar2;
  
  if (0x7eff0 < size) {
    size = 0x7eff0;
  }
  uVar2 = (uint)DAT_800ab234;
  uVar1 = DAT_800ab244 & 0x1f;
  _spu_Fw(addr,size);
  DAT_800ab234 = _spu_FsetRXXa(0xffffffff,(uVar2 << uVar1) + size);
  if (DAT_800ab254 == 0) {
    DAT_800ab250 = 0;
  }
  return size;
}


