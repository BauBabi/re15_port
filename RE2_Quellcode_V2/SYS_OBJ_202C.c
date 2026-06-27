/* SYS_OBJ_202C @ 0x80091f0c  (Ghidra headless, raw MIPS overlay) */

undefined4 SYS_OBJ_202C(short param_1)

{
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 *unaff_s1;
  undefined4 *unaff_s2;
  int unaff_s5;
  
  *(short *)((int)unaff_s1 + 6) = param_1;
  iVar4 = (int)*(short *)(unaff_s1 + 1) * (int)param_1 + 1;
  iVar5 = iVar4 - (iVar4 >> 0x1f) >> 5;
  if (iVar4 / 2 < 1) {
    uVar1 = SYS_OBJ_21A4();
    return uVar1;
  }
  iVar4 = iVar4 / 2 + iVar5 * -0x10;
  uVar2 = *(uint *)PTR_GPU_REG1_800b27d4;
  while( true ) {
    if ((uVar2 & 0x4000000) != 0) {
      uVar1 = 0xa0000000;
      *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x4000000;
      *(undefined4 *)PTR_GPU_REG0_800b27d0 = 0x1000000;
      if (unaff_s5 != 0) {
        uVar1 = 0xb0000000;
      }
      *(undefined4 *)PTR_GPU_REG0_800b27d0 = uVar1;
      *(undefined4 *)PTR_GPU_REG0_800b27d0 = *unaff_s1;
      *(undefined4 *)PTR_GPU_REG0_800b27d0 = unaff_s1[1];
      while (iVar4 = iVar4 + -1, iVar4 != -1) {
        uVar1 = *unaff_s2;
        unaff_s2 = unaff_s2 + 1;
        *(undefined4 *)PTR_GPU_REG0_800b27d0 = uVar1;
      }
      if (iVar5 != 0) {
        *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x4000002;
        *(undefined4 **)PTR_DMA_GPU_MADR_800b27d8 = unaff_s2;
        *(uint *)PTR_DMA_GPU_BCR_800b27dc = iVar5 << 0x10 | 0x10;
        *(undefined4 *)PTR_DMA_GPU_CHCR_800b27e0 = 0x1000201;
      }
      return 0;
    }
    iVar3 = get_alarm();
    if (iVar3 != 0) break;
    uVar2 = *(uint *)PTR_GPU_REG1_800b27d4;
  }
  return 0xffffffff;
}


