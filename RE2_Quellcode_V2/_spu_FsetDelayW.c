/* _spu_FsetDelayW @ 0x80079134  (Ghidra headless, raw MIPS overlay) */

void _spu_FsetDelayW(void)

{
  *(uint *)PTR_SPU_DELAY_800ab230 = *(uint *)PTR_SPU_DELAY_800ab230 & 0xf0ffffff | 0x20000000;
  return;
}


