undefined4 _dws(int param_1,dword *param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  short sVar4;
  short sVar5;
  int iVar6;
  
  set_alarm();
  sVar4 = *(short *)(param_1 + 4);
  if (sVar4 < 0) {
    sVar4 = 0;
  }
  else if (DAT_8007e354 < sVar4) {
    uVar1 = SYS_OBJ_1C94();
    return uVar1;
  }
  sVar5 = *(short *)(param_1 + 6);
  *(short *)(param_1 + 4) = sVar4;
  if (sVar5 < 0) {
    sVar5 = 0;
  }
  else if (DAT_8007e356 < sVar5) {
    uVar1 = SYS_OBJ_1CDC(DAT_8007e356);
    return uVar1;
  }
  *(short *)(param_1 + 6) = sVar5;
  iVar2 = (int)*(short *)(param_1 + 4) * (int)sVar5 + 1;
  iVar6 = iVar2 - (iVar2 >> 0x1f) >> 5;
  if (0 < iVar2 / 2) {
    iVar2 = iVar2 / 2 + iVar6 * -0x10;
    do {
      if ((GPU_REG1 & 0x4000000) != 0) {
        GPU_REG1 = 0x4000000;
        GPU_REG0 = *(dword *)(param_1 + 4);
        while (iVar2 = iVar2 + -1, iVar2 != -1) {
          GPU_REG0 = *param_2;
          param_2 = param_2 + 1;
        }
        if (iVar6 != 0) {
          GPU_REG1 = 0x4000002;
          DMA_GPU_BCR = iVar6 << 0x10 | 0x10;
          DMA_GPU_CHCR = 0x1000201;
          DMA_GPU_MADR = (dword)param_2;
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
