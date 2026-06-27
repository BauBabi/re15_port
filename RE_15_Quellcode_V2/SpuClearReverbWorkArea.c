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
    iVar3 = _SpuIsInAllocateArea_((&DAT_80077224)[mode]);
    iVar2 = DAT_80076d74;
    if (iVar3 == 0) {
      if (mode == 0) {
        lVar4 = S_CRWA_OBJ_9C();
        return lVar4;
      }
      iVar3 = (&DAT_80077224)[mode];
      uVar5 = 0x10000 - iVar3 << (DAT_80076d80 & 0x1f);
      bVar1 = DAT_80076d74 == 1;
      if (bVar1) {
        DAT_80076d74 = 0;
      }
      if (DAT_80076d90 != 0) {
        local_28 = DAT_80076d90;
        DAT_80076d90 = 0;
      }
      if (uVar5 < 0x401) {
        _spu_t(2,iVar3 << (DAT_80076d80 & 0x1f));
        _spu_t(1);
        _spu_t(3,&DAT_80076e20,uVar5);
        WaitEvent(DAT_80076e18);
        if (bVar1) {
          DAT_80076d74 = iVar2;
        }
        if (local_28 != 0) {
          DAT_80076d90 = local_28;
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
