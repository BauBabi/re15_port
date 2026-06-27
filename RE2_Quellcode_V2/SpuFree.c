/* SpuFree @ 0x80084f44  (Ghidra headless, raw MIPS overlay) */

void SpuFree(ulong addr)

{
  uint *puVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < DAT_800ab6ec) {
    puVar1 = DAT_800ab6f4;
    do {
      if ((*puVar1 & 0x40000000) != 0) break;
      iVar2 = iVar2 + 1;
      if (*puVar1 == addr) {
        *puVar1 = addr | 0x80000000;
        S_M_F_OBJ_64();
        return;
      }
      puVar1 = puVar1 + 2;
    } while (iVar2 < DAT_800ab6ec);
  }
  _spu_gcSPU();
  return;
}


