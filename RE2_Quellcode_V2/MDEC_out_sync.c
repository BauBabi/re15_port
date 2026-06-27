/* MDEC_out_sync @ 0x80094df0  (Ghidra headless, raw MIPS overlay) */

undefined4 MDEC_out_sync(void)

{
  uint uVar1;
  undefined4 uVar2;
  int local_10;
  
  local_10 = 0x100000;
  uVar1 = *(uint *)PTR_DMA_MDEC_OUT_CHCR_800b2944;
  while( true ) {
    if ((uVar1 & 0x1000000) == 0) {
      return 0;
    }
    local_10 = local_10 + -1;
    if (local_10 == -1) break;
    uVar1 = *(uint *)PTR_DMA_MDEC_OUT_CHCR_800b2944;
  }
  timeout("MDEC_out_sync");
  uVar2 = LIBPRESS_OBJ_608();
  return uVar2;
}


