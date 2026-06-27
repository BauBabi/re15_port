/* _spu_FsetDelayR @ 0x80079160  (Ghidra headless, raw MIPS overlay) */

void _spu_FsetDelayR(void)

{
  *(uint *)PTR_SPU_DELAY_800ab230 = *(uint *)PTR_SPU_DELAY_800ab230 & 0xf0ffffff | 0x22000000;
  return;
}


