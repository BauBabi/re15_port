/* setIntrDMA @ 0x80086a90  (Ghidra headless, raw MIPS overlay) */

int setIntrDMA(int param_1,int param_2)

{
  undefined *puVar1;
  int *piVar2;
  int iVar3;
  
  puVar1 = PTR_DMA_DICR_800acd08;
  piVar2 = &DAT_800acd0c + param_1;
  iVar3 = *piVar2;
  if (param_2 != iVar3) {
    if (param_2 != 0) {
      *piVar2 = param_2;
      *(uint *)puVar1 = *(uint *)puVar1 & 0xffffff | 1 << (param_1 + 0x10U & 0x1f) | 0x800000U;
      iVar3 = INTR_DMA_OBJ_274();
      return iVar3;
    }
    *piVar2 = 0;
    *(uint *)puVar1 = (*(uint *)puVar1 & 0xffffff | 0x800000) & ~(1 << (param_1 + 0x10U & 0x1f));
  }
  return iVar3;
}


