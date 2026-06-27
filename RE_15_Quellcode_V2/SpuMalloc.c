long SpuMalloc(long size)

{
  uint uVar1;
  uint uVar2;
  long lVar3;
  uint *puVar4;
  int iVar5;
  
  iVar5 = 0;
  if (DAT_80076dcc == 0) {
    lVar3 = S_M_M_OBJ_54();
    return lVar3;
  }
  if ((size & ~DAT_80076d88) != 0) {
    size = size + DAT_80076d88;
  }
  uVar2 = DAT_80076d80 & 0x1f;
  uVar1 = DAT_80076d80 & 0x1f;
  if ((*DAT_80076dbc & 0x40000000) != 0) {
    lVar3 = S_M_M_OBJ_12C();
    return lVar3;
  }
  _spu_gcSPU();
  puVar4 = DAT_80076dbc;
  if (0 < DAT_80076db4) {
    do {
      if (((*puVar4 & 0x40000000) != 0) ||
         (((*puVar4 & 0x80000000) != 0 && ((uint)((size >> uVar2) << uVar1) <= puVar4[1])))) {
        lVar3 = S_M_M_OBJ_12C();
        return lVar3;
      }
      iVar5 = iVar5 + 1;
      puVar4 = puVar4 + 2;
    } while (iVar5 < DAT_80076db4);
  }
  return -1;
}
