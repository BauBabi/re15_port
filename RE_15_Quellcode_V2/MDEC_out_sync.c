undefined4 MDEC_out_sync(void)

{
  undefined4 uVar1;
  int local_10;
  
  local_10 = 0x100000;
  do {
    if ((DMA_MDEC_OUT_CHCR & 0x1000000) == 0) {
      return 0;
    }
    local_10 = local_10 + -1;
  } while (local_10 != -1);
  timeout("MDEC_out_sync");
  uVar1 = LIBPRESS_OBJ_5D0();
  return uVar1;
}
