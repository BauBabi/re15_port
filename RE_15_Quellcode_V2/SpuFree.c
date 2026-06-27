void SpuFree(ulong addr)

{
  uint *puVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < DAT_80076db4) {
    puVar1 = DAT_80076dbc;
    do {
      if ((*puVar1 & 0x40000000) != 0) break;
      iVar2 = iVar2 + 1;
      if (*puVar1 == addr) {
        *puVar1 = addr | 0x80000000;
        S_M_F_OBJ_64();
        return;
      }
      puVar1 = puVar1 + 2;
    } while (iVar2 < DAT_80076db4);
  }
  _spu_gcSPU();
  return;
}
