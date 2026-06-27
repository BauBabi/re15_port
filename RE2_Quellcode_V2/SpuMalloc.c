/* SpuMalloc @ 0x800857a4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x800858dc) */
/* WARNING: Removing unreachable block (ram,0x800859a0) */
/* WARNING: Removing unreachable block (ram,0x800859b4) */
/* WARNING: Removing unreachable block (ram,0x800859d4) */
/* WARNING: Removing unreachable block (ram,0x80085a10) */
/* WARNING: Removing unreachable block (ram,0x80085900) */
/* WARNING: Removing unreachable block (ram,0x80085918) */
/* WARNING: Removing unreachable block (ram,0x80085930) */

long SpuMalloc(long size)

{
  uint uVar1;
  uint uVar2;
  long lVar3;
  uint *puVar4;
  int iVar5;
  
  iVar5 = 0;
  if (DAT_800ab290 == 0) {
    lVar3 = S_M_M_OBJ_54();
    return lVar3;
  }
  if ((size & ~DAT_800ab24c) != 0) {
    size = size + DAT_800ab24c;
  }
  uVar2 = DAT_800ab244 & 0x1f;
  uVar1 = DAT_800ab244 & 0x1f;
  if ((*DAT_800ab6f4 & 0x40000000) != 0) {
    lVar3 = S_M_M_OBJ_12C();
    return lVar3;
  }
  _spu_gcSPU();
  puVar4 = DAT_800ab6f4;
  if (0 < DAT_800ab6ec) {
    do {
      if (((*puVar4 & 0x40000000) != 0) ||
         (((*puVar4 & 0x80000000) != 0 && ((uint)((size >> uVar2) << uVar1) <= puVar4[1])))) {
        lVar3 = S_M_M_OBJ_12C();
        return lVar3;
      }
      iVar5 = iVar5 + 1;
      puVar4 = puVar4 + 2;
    } while (iVar5 < DAT_800ab6ec);
  }
  return -1;
}


