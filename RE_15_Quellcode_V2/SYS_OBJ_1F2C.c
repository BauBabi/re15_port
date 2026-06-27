undefined4 SYS_OBJ_1F2C(undefined2 param_1)

{
  int in_v0;
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int unaff_s1;
  dword *unaff_s2;
  
  *(undefined2 *)(unaff_s1 + 6) = param_1;
  iVar1 = (int)*(short *)(unaff_s1 + 4) * (in_v0 >> 0x10) + 1;
  iVar4 = iVar1 - (iVar1 >> 0x1f) >> 5;
  if (iVar1 / 2 < 1) {
    uVar2 = SYS_OBJ_20F0();
    return uVar2;
  }
  iVar1 = iVar1 / 2 + iVar4 * -0x10;
  do {
    if ((GPU_REG1 & 0x4000000) != 0) {
      GPU_REG1 = 0x4000000;
      GPU_REG0 = *(dword *)(unaff_s1 + 4);
      do {
        iVar3 = get_alarm();
        if (iVar3 != 0) {
          return 0xffffffff;
        }
      } while ((GPU_REG1 & 0x8000000) == 0);
      while (iVar1 = iVar1 + -1, iVar1 != -1) {
        *unaff_s2 = GPU_REG0;
        unaff_s2 = unaff_s2 + 1;
      }
      if (iVar4 != 0) {
        GPU_REG1 = 0x4000003;
        DMA_GPU_BCR = iVar4 << 0x10 | 0x10;
        DMA_GPU_CHCR = 0x1000200;
        DMA_GPU_MADR = (dword)unaff_s2;
      }
      return 0;
    }
    iVar3 = get_alarm();
  } while (iVar3 == 0);
  return 0xffffffff;
}
