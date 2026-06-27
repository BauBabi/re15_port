/* S_M_INT_OBJ_278 @ 0x80085238  (Ghidra headless, raw MIPS overlay) */

void S_M_INT_OBJ_278(void)

{
  int iVar1;
  int iVar2;
  uint *puVar3;
  int iVar4;
  int iVar5;
  
  iVar1 = DAT_800ab6f4;
  iVar4 = DAT_800ab6f0 + -1;
  if (-1 < iVar4) {
    puVar3 = (uint *)(iVar4 * 8 + DAT_800ab6f4);
    do {
      if ((*puVar3 & 0x80000000) == 0) {
        return;
      }
      *puVar3 = *puVar3 & 0xfffffff | 0x40000000;
      iVar2 = DAT_800ab6f0 * 8;
      iVar5 = iVar4 + -1;
      DAT_800ab6f0 = iVar4;
      puVar3[1] = puVar3[1] + *(int *)(iVar2 + iVar1 + 4);
      puVar3 = puVar3 + -2;
      iVar4 = iVar5;
    } while (-1 < iVar5);
  }
  return;
}


