void MDEC_reset(int param_1)

{
  if (param_1 == 0) {
    DMA_MDEC_IN_CHCR = 0;
    DMA_MDEC_OUT_CHCR = 0;
    MDEC_REG1 = 0x60000000;
    MDEC_in(&DAT_8007e470,0x20);
    MDEC_in(&DAT_8007e4f4,0x20);
    LIBPRESS_OBJ_37C();
    return;
  }
  if (param_1 != 1) {
    LIBPRESS_OBJ_36C();
    return;
  }
  DMA_MDEC_IN_CHCR = 0;
  DMA_MDEC_OUT_CHCR = 0;
  MDEC_REG1 = 0x60000000;
  LIBPRESS_OBJ_37C();
  return;
}
