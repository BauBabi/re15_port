/* SYS_OBJ_25C8 @ 0x800924a8  (Ghidra headless, raw MIPS overlay) */

uint SYS_OBJ_25C8(void)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *unaff_s0;
  int unaff_s1;
  undefined4 unaff_s2;
  code *unaff_s3;
  
  while ((DAT_800b2804 + 1 & 0x3f) == DAT_800b2808) {
    iVar5 = get_alarm();
    if (iVar5 != 0) {
      return 0xffffffff;
    }
    _exeque();
  }
  DAT_800b280c = SetIntrMask(0);
  DAT_800b2708 = 1;
  if ((DAT_800b2701 != '\0') &&
     (((DAT_800b2804 != DAT_800b2808 || ((*(uint *)PTR_DMA_GPU_CHCR_800b27e0 & 0x1000000) != 0)) ||
      (DAT_800b270c != 0)))) {
    DMACallback(2,_exeque);
    iVar5 = 0;
    if (unaff_s1 == 0) {
      *(undefined4 **)(&DAT_800e899c + DAT_800b2804 * 0x60) = unaff_s0;
      *(undefined4 *)(&DAT_800e89a0 + DAT_800b2804 * 0x60) = unaff_s2;
      *(code **)(&DAT_800e8998 + DAT_800b2804 * 0x60) = unaff_s3;
      DAT_800b2804 = DAT_800b2804 + 1 & 0x3f;
      SetIntrMask(DAT_800b280c);
      _exeque();
      return DAT_800b2804 - DAT_800b2808 & 0x3f;
    }
    while( true ) {
      iVar2 = unaff_s1;
      if (unaff_s1 < 0) {
        iVar2 = unaff_s1 + 3;
      }
      iVar3 = iVar5 * 4;
      if (iVar2 >> 2 <= iVar5) break;
      uVar4 = *unaff_s0;
      unaff_s0 = unaff_s0 + 1;
      iVar5 = iVar5 + 1;
      *(undefined4 *)(&DAT_800e89a4 + iVar3 + DAT_800b2804 * 0x60) = uVar4;
    }
    *(undefined **)(&DAT_800e899c + DAT_800b2804 * 0x60) = &DAT_800e89a4 + DAT_800b2804 * 0x60;
    uVar1 = SYS_OBJ_27A8();
    return uVar1;
  }
  do {
  } while ((*(uint *)PTR_GPU_REG1_800b27d4 & 0x4000000) == 0);
  (*unaff_s3)();
  DAT_800b27f4 = unaff_s3;
  DAT_800b27f8 = unaff_s0;
  DAT_800b27fc = unaff_s2;
  SetIntrMask(DAT_800b280c);
  uVar1 = SYS_OBJ_283C();
  return uVar1;
}


