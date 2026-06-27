uint _exeque(void)

{
  uint uVar1;
  
  uVar1 = 1;
  if ((DMA_GPU_CHCR & 0x1000000) == 0) {
    DAT_8007e460 = SetIntrMask(0);
    if (DAT_8007e454 != DAT_8007e458) {
      do {
        if ((DMA_GPU_CHCR & 0x1000000) != 0) break;
        if (((DAT_8007e458 + 1 & 0x3f) == DAT_8007e454) && (DAT_8007e35c == (code *)0x0)) {
          DMACallback(2,0);
        }
        do {
        } while ((GPU_REG1 & 0x4000000) == 0);
        (**(code **)(&DAT_800b9cb4 + DAT_8007e458 * 0x60))
                  (*(undefined4 *)(&DAT_800b9cb8 + DAT_8007e458 * 0x60),
                   *(undefined4 *)(&DAT_800b9cbc + DAT_8007e458 * 0x60));
        DAT_8007e444 = *(undefined4 *)(&DAT_800b9cb4 + DAT_8007e458 * 0x60);
        DAT_8007e448 = *(undefined4 *)(&DAT_800b9cb8 + DAT_8007e458 * 0x60);
        DAT_8007e44c = *(undefined4 *)(&DAT_800b9cbc + DAT_8007e458 * 0x60);
        DAT_8007e458 = DAT_8007e458 + 1 & 0x3f;
      } while (DAT_8007e454 != DAT_8007e458);
    }
    SetIntrMask(DAT_8007e460);
    if ((((DAT_8007e454 == DAT_8007e458) && ((DMA_GPU_CHCR & 0x1000000) == 0)) &&
        (DAT_8007e358 != 0)) && (DAT_8007e35c != (code *)0x0)) {
      DAT_8007e358 = 0;
      (*DAT_8007e35c)();
    }
    uVar1 = DAT_8007e454 - DAT_8007e458 & 0x3f;
  }
  return uVar1;
}
