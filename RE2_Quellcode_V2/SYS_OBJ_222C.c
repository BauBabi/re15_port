/* SYS_OBJ_222C @ 0x8009210c  (Ghidra headless, raw MIPS overlay) */

undefined4 SYS_OBJ_222C(void)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  undefined2 in_v1;
  short sVar5;
  int iVar6;
  undefined4 *unaff_s1;
  undefined4 *unaff_s2;
  
  sVar5 = *(short *)((int)unaff_s1 + 6);
  iVar6 = (int)sVar5;
  *(undefined2 *)(unaff_s1 + 1) = in_v1;
  if (iVar6 < 0) {
    sVar5 = 0;
    iVar1 = 0;
  }
  else {
    iVar1 = iVar6 << 0x10;
    if (DAT_800b2706 < iVar6) {
      uVar2 = SYS_OBJ_2264((int)DAT_800b2706);
      return uVar2;
    }
  }
  *(short *)((int)unaff_s1 + 6) = sVar5;
  iVar6 = (int)*(short *)(unaff_s1 + 1) * (iVar1 >> 0x10) + 1;
  iVar1 = iVar6 - (iVar6 >> 0x1f) >> 5;
  if (0 < iVar6 / 2) {
    iVar6 = iVar6 / 2 + iVar1 * -0x10;
    uVar3 = *(uint *)PTR_GPU_REG1_800b27d4;
    while ((uVar3 & 0x4000000) == 0) {
      iVar4 = get_alarm();
      if (iVar4 != 0) {
        return 0xffffffff;
      }
      uVar3 = *(uint *)PTR_GPU_REG1_800b27d4;
    }
    *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x4000000;
    *(undefined4 *)PTR_GPU_REG0_800b27d0 = 0x1000000;
    *(undefined4 *)PTR_GPU_REG0_800b27d0 = 0xc0000000;
    *(undefined4 *)PTR_GPU_REG0_800b27d0 = *unaff_s1;
    *(undefined4 *)PTR_GPU_REG0_800b27d0 = unaff_s1[1];
    uVar3 = *(uint *)PTR_GPU_REG1_800b27d4;
    while( true ) {
      if ((uVar3 & 0x8000000) != 0) {
        while (iVar6 = iVar6 + -1, iVar6 != -1) {
          *unaff_s2 = *(undefined4 *)PTR_GPU_REG0_800b27d0;
          unaff_s2 = unaff_s2 + 1;
        }
        if (iVar1 != 0) {
          *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x4000003;
          *(undefined4 **)PTR_DMA_GPU_MADR_800b27d8 = unaff_s2;
          *(uint *)PTR_DMA_GPU_BCR_800b27dc = iVar1 << 0x10 | 0x10;
          *(undefined4 *)PTR_DMA_GPU_CHCR_800b27e0 = 0x1000200;
        }
        return 0;
      }
      iVar4 = get_alarm();
      if (iVar4 != 0) break;
      uVar3 = *(uint *)PTR_GPU_REG1_800b27d4;
    }
    return 0xffffffff;
  }
  uVar2 = SYS_OBJ_242C();
  return uVar2;
}


