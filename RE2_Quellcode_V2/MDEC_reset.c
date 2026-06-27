/* MDEC_reset @ 0x80094b38  (Ghidra headless, raw MIPS overlay) */

void MDEC_reset(int param_1)

{
  undefined4 uVar1;
  
  if (param_1 == 0) {
    *(undefined4 *)PTR_MDEC_REG1_800b2964 = 0x80000000;
    *(undefined4 *)PTR_DMA_MDEC_IN_CHCR_800b2938 = 0;
    *(undefined4 *)PTR_DMA_MDEC_OUT_CHCR_800b2944 = 0;
    *(undefined4 *)PTR_MDEC_REG1_800b2964 = 0x60000000;
    MDEC_in(&DAT_800b2820,0x20);
    MDEC_in(&DAT_800b28a4,0x20);
    LIBPRESS_OBJ_3B4();
    return;
  }
  if (param_1 != 1) {
    LIBPRESS_OBJ_3A4();
    return;
  }
  *(undefined4 *)PTR_MDEC_REG1_800b2964 = 0x80000000;
  *(undefined4 *)PTR_DMA_MDEC_IN_CHCR_800b2938 = 0;
  *(undefined4 *)PTR_DMA_MDEC_OUT_CHCR_800b2944 = 0;
  uVar1 = *(undefined4 *)PTR_DMA_MDEC_OUT_CHCR_800b2944;
  *(undefined4 *)PTR_MDEC_REG1_800b2964 = 0x60000000;
  LIBPRESS_OBJ_3B4();
  return;
}


