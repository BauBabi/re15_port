/* SpuSetTransferStartAddr @ 0x80085b8c  (Ghidra headless, raw MIPS overlay) */

ulong SpuSetTransferStartAddr(ulong addr)

{
  ulong uVar1;
  
  if (addr - 0x1010 < 0x7efe9) {
    DAT_800ab234 = _spu_FsetRXXa(0xffffffff);
    uVar1 = S_STSA_OBJ_48();
    return uVar1;
  }
  return 0;
}


