/* _spu_Fr_ @ 0x80078b80  (Ghidra headless, raw MIPS overlay) */

void _spu_Fr_(undefined4 param_1,undefined2 param_2,int param_3)

{
  *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1a6) = param_2;
  _spu_Fw1ts();
  *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
       *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 0x30;
  _spu_Fw1ts();
  _spu_FsetDelayR();
  *(undefined4 *)PTR_DMA_SPU_MADR_800ab220 = param_1;
  *(uint *)PTR_DMA_SPU_BCR_800ab224 = param_3 << 0x10 | 0x10;
  DAT_800ab26c = 1;
  *(undefined4 *)PTR_DMA_SPU_CHCR_800ab228 = 0x1000200;
  return;
}


