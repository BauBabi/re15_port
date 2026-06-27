/* SpuClearReverbWorkArea @ 0x800784c4  (Ghidra headless, raw MIPS overlay) */

long SpuClearReverbWorkArea(long mode)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  long lVar4;
  uint uVar5;
  int local_28;
  
  local_28 = 0;
  if ((uint)mode < 10) {
    iVar3 = _SpuIsInAllocateArea_((&DAT_800ab6f8)[mode]);
    iVar2 = DAT_800ab238;
    if (iVar3 == 0) {
      if (mode == 0) {
        lVar4 = S_CRWA_OBJ_9C();
        return lVar4;
      }
      iVar3 = (&DAT_800ab6f8)[mode];
      uVar5 = 0x10000 - iVar3 << (DAT_800ab244 & 0x1f);
      bVar1 = DAT_800ab238 == 1;
      if (bVar1) {
        DAT_800ab238 = 0;
      }
      if (DAT_800ab254 != 0) {
        local_28 = DAT_800ab254;
        DAT_800ab254 = 0;
      }
      if (uVar5 < 0x401) {
        _spu_t(2,iVar3 << (DAT_800ab244 & 0x1f));
        _spu_t(1);
        _spu_t(3,&DAT_800ab2e4,uVar5);
        WaitEvent(DAT_800ab280);
        if (bVar1) {
          DAT_800ab238 = iVar2;
        }
        if (local_28 != 0) {
          DAT_800ab254 = local_28;
        }
        return 0;
      }
      lVar4 = S_CRWA_OBJ_100();
      return lVar4;
    }
  }
  lVar4 = S_CRWA_OBJ_174();
  return lVar4;
}


