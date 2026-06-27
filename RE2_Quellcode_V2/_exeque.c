/* _exeque @ 0x8009273c  (Ghidra headless, raw MIPS overlay) */

uint _exeque(void)

{
  uint uVar1;
  
  uVar1 = 1;
  if ((*(uint *)PTR_DMA_GPU_CHCR_800b27e0 & 0x1000000) == 0) {
    DAT_800b2810 = SetIntrMask(0);
    if (DAT_800b2804 != DAT_800b2808) {
      uVar1 = *(uint *)PTR_DMA_GPU_CHCR_800b27e0;
      while ((uVar1 & 0x1000000) == 0) {
        if (((DAT_800b2808 + 1 & 0x3f) == DAT_800b2804) && (DAT_800b270c == (code *)0x0)) {
          DMACallback(2,0);
        }
        uVar1 = *(uint *)PTR_GPU_REG1_800b27d4;
        while ((uVar1 & 0x4000000) == 0) {
          uVar1 = *(uint *)PTR_GPU_REG1_800b27d4;
        }
        (**(code **)(&DAT_800e8998 + DAT_800b2808 * 0x60))
                  (*(undefined4 *)(&DAT_800e899c + DAT_800b2808 * 0x60),
                   *(undefined4 *)(&DAT_800e89a0 + DAT_800b2808 * 0x60));
        DAT_800b27f4 = *(undefined4 *)(&DAT_800e8998 + DAT_800b2808 * 0x60);
        DAT_800b27f8 = *(undefined4 *)(&DAT_800e899c + DAT_800b2808 * 0x60);
        DAT_800b27fc = *(undefined4 *)(&DAT_800e89a0 + DAT_800b2808 * 0x60);
        DAT_800b2808 = DAT_800b2808 + 1 & 0x3f;
        if (DAT_800b2804 == DAT_800b2808) break;
        uVar1 = *(uint *)PTR_DMA_GPU_CHCR_800b27e0;
      }
    }
    SetIntrMask(DAT_800b2810);
    if ((((DAT_800b2804 == DAT_800b2808) && ((*(uint *)PTR_DMA_GPU_CHCR_800b27e0 & 0x1000000) == 0))
        && (DAT_800b2708 != 0)) && (DAT_800b270c != (code *)0x0)) {
      DAT_800b2708 = 0;
      (*DAT_800b270c)();
    }
    uVar1 = DAT_800b2804 - DAT_800b2808 & 0x3f;
  }
  return uVar1;
}


