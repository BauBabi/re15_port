undefined4 get_alarm(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_80061fc0(0xffffffff);
  iVar1 = DAT_8007e46c;
  if ((iVar2 <= DAT_8007e468) && (DAT_8007e46c = DAT_8007e46c + 1, iVar1 < 0xf0001)) {
    return 0;
  }
  printf("GPU timeout:que=%d,stat=%08x,chcr=%08x,madr=%08x,",DAT_8007e454 - DAT_8007e458 & 0x3f,
         GPU_REG1,DMA_GPU_CHCR,DMA_GPU_MADR);
  printf("func=(%08x)(%08x,%08x)\n",DAT_8007e444,DAT_8007e448,DAT_8007e44c);
  DAT_8007e464 = SetIntrMask(0);
  DAT_8007e458 = 0;
  DAT_8007e454 = 0;
  DMA_GPU_CHCR = 0x401;
  DMA_DPCR = DMA_DPCR | 0x800;
  GPU_REG1 = 0x1000000;
  SetIntrMask(DAT_8007e464);
  uVar3 = SYS_OBJ_2C34();
  return uVar3;
}
