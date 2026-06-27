undefined4 SYS_OBJ_1EE4(void)

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
    uVar1 = SYS_OBJ_1F2C(DAT_8007e356);
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
        do {
          iVar3 = get_alarm();
          if (iVar3 != 0) {
            return 0xffffffff;
          }
        } while ((GPU_REG1 & 0x8000000) == 0);
        while (iVar2 = iVar2 + -1, iVar2 != -1) {
          *unaff_s2 = GPU_REG0;
          unaff_s2 = unaff_s2 + 1;
        }
        if (iVar5 != 0) {
          GPU_REG1 = 0x4000003;
          DMA_GPU_BCR = iVar5 << 0x10 | 0x10;
          DMA_GPU_CHCR = 0x1000200;
          DMA_GPU_MADR = (dword)unaff_s2;
        }
        return 0;
      }
      iVar3 = get_alarm();
    } while (iVar3 == 0);
    return 0xffffffff;
  }
  uVar1 = SYS_OBJ_20F0();
  return uVar1;
}
