undefined4 SYS_OBJ_1C94(void)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined2 in_v1;
  short sVar4;
  int iVar5;
  int unaff_s1;
  dword *unaff_s2;
  
  sVar4 = *(short *)(unaff_s1 + 6);
  *(undefined2 *)(unaff_s1 + 4) = in_v1;
  if (sVar4 < 0) {
    sVar4 = 0;
  }
  else if (DAT_8007e356 < sVar4) {
    uVar1 = SYS_OBJ_1CDC(DAT_8007e356);
    return uVar1;
  }
  *(short *)(unaff_s1 + 6) = sVar4;
  iVar2 = (int)*(short *)(unaff_s1 + 4) * (int)sVar4 + 1;
  iVar5 = iVar2 - (iVar2 >> 0x1f) >> 5;
  if (0 < iVar2 / 2) {
    iVar2 = iVar2 / 2 + iVar5 * -0x10;
    do {
      if ((GPU_REG1 & 0x4000000) != 0) {
        GPU_REG1 = 0x4000000;
        GPU_REG0 = *(dword *)(unaff_s1 + 4);
        while (iVar2 = iVar2 + -1, iVar2 != -1) {
          GPU_REG0 = *unaff_s2;
          unaff_s2 = unaff_s2 + 1;
        }
        if (iVar5 != 0) {
          GPU_REG1 = 0x4000002;
          DMA_GPU_BCR = iVar5 << 0x10 | 0x10;
          DMA_GPU_CHCR = 0x1000201;
          DMA_GPU_MADR = (dword)unaff_s2;
        }
        return 0;
      }
      iVar3 = get_alarm();
    } while (iVar3 == 0);
    return 0xffffffff;
  }
  uVar1 = SYS_OBJ_1E50();
  return uVar1;
}
