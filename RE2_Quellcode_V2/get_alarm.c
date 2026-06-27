/* get_alarm @ 0x80092d00  (Ghidra headless, raw MIPS overlay) */

undefined4 get_alarm(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = VSync(-1);
  iVar1 = DAT_800b281c;
  if ((iVar2 <= DAT_800b2818) && (DAT_800b281c = DAT_800b281c + 1, iVar1 < 0xf0001)) {
    return 0;
  }
  uVar3 = *(undefined4 *)PTR_GPU_REG1_800b27d4;
  printf("GPU timeout:que=%d,stat=%08x,chcr=%08x,madr=%08x,",DAT_800b2804 - DAT_800b2808 & 0x3f,
         *(undefined4 *)PTR_GPU_REG1_800b27d4,*(undefined4 *)PTR_DMA_GPU_CHCR_800b27e0,
         *(undefined4 *)PTR_DMA_GPU_MADR_800b27d8);
  printf("func=(%08x)(%08x,%08x)\n",DAT_800b27f4,DAT_800b27f8,DAT_800b27fc);
  DAT_800b2814 = SetIntrMask(0);
  DAT_800b2808 = 0;
  DAT_800b2804 = 0;
  *(undefined4 *)PTR_DMA_GPU_CHCR_800b27e0 = 0x401;
  *(uint *)PTR_DMA_DPCR_800b27f0 = *(uint *)PTR_DMA_DPCR_800b27f0 | 0x800;
  *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x2000000;
  *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x1000000;
  SetIntrMask(DAT_800b2814);
  uVar3 = SYS_OBJ_2F7C();
  return uVar3;
}


