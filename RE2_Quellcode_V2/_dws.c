/* _dws @ 0x80091e70  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x80091fd4) */

undefined4 _dws(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  short sVar5;
  short sVar6;
  int iVar7;
  
  set_alarm();
  sVar5 = *(short *)(param_1 + 1);
  if (sVar5 < 0) {
    sVar5 = 0;
  }
  else if ((int)DAT_800b2704 < (int)sVar5) {
    uVar1 = SYS_OBJ_1FF4((int)DAT_800b2704);
    return uVar1;
  }
  sVar6 = *(short *)((int)param_1 + 6);
  iVar7 = (int)sVar6;
  *(short *)(param_1 + 1) = sVar5;
  if (iVar7 < 0) {
    sVar6 = 0;
    iVar2 = 0;
  }
  else {
    iVar2 = iVar7 << 0x10;
    if (DAT_800b2706 < iVar7) {
      uVar1 = SYS_OBJ_202C((int)DAT_800b2706);
      return uVar1;
    }
  }
  *(short *)((int)param_1 + 6) = sVar6;
  iVar7 = (int)*(short *)(param_1 + 1) * (iVar2 >> 0x10) + 1;
  iVar2 = iVar7 - (iVar7 >> 0x1f) >> 5;
  if (0 < iVar7 / 2) {
    iVar7 = iVar7 / 2 + iVar2 * -0x10;
    uVar3 = *(uint *)PTR_GPU_REG1_800b27d4;
    while( true ) {
      if ((uVar3 & 0x4000000) != 0) {
        *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x4000000;
        *(undefined4 *)PTR_GPU_REG0_800b27d0 = 0x1000000;
        *(undefined4 *)PTR_GPU_REG0_800b27d0 = 0xa0000000;
        *(undefined4 *)PTR_GPU_REG0_800b27d0 = *param_1;
        *(undefined4 *)PTR_GPU_REG0_800b27d0 = param_1[1];
        while (iVar7 = iVar7 + -1, iVar7 != -1) {
          uVar1 = *param_2;
          param_2 = param_2 + 1;
          *(undefined4 *)PTR_GPU_REG0_800b27d0 = uVar1;
        }
        if (iVar2 != 0) {
          *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x4000002;
          *(undefined4 **)PTR_DMA_GPU_MADR_800b27d8 = param_2;
          *(uint *)PTR_DMA_GPU_BCR_800b27dc = iVar2 << 0x10 | 0x10;
          *(undefined4 *)PTR_DMA_GPU_CHCR_800b27e0 = 0x1000201;
        }
        return 0;
      }
      iVar4 = get_alarm();
      if (iVar4 != 0) break;
      uVar3 = *(uint *)PTR_GPU_REG1_800b27d4;
    }
    return 0xffffffff;
  }
  uVar1 = SYS_OBJ_21A4();
  return uVar1;
}


