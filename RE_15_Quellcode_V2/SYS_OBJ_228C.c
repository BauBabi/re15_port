uint SYS_OBJ_228C(void)

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
  
  while ((DAT_8007e454 + 1 & 0x3f) == DAT_8007e458) {
    iVar5 = get_alarm();
    if (iVar5 != 0) {
      return 0xffffffff;
    }
    _exeque();
  }
  DAT_8007e45c = SetIntrMask(0);
  DAT_8007e358 = 1;
  if ((DAT_8007e351 != '\0') &&
     (((DAT_8007e454 != DAT_8007e458 || ((DMA_GPU_CHCR & 0x1000000) != 0)) || (DAT_8007e35c != 0))))
  {
    DMACallback(2,_exeque);
    iVar5 = 0;
    if (unaff_s1 == 0) {
      *(undefined4 **)(&DAT_800b9cb8 + DAT_8007e454 * 0x60) = unaff_s0;
      *(undefined4 *)(&DAT_800b9cbc + DAT_8007e454 * 0x60) = unaff_s2;
      *(code **)(&DAT_800b9cb4 + DAT_8007e454 * 0x60) = unaff_s3;
      DAT_8007e454 = DAT_8007e454 + 1 & 0x3f;
      SetIntrMask(DAT_8007e45c);
      _exeque();
      return DAT_8007e454 - DAT_8007e458 & 0x3f;
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
      *(undefined4 *)(&DAT_800b9cc0 + iVar3 + DAT_8007e454 * 0x60) = uVar4;
    }
    *(undefined **)(&DAT_800b9cb8 + DAT_8007e454 * 0x60) = &DAT_800b9cc0 + DAT_8007e454 * 0x60;
    uVar1 = SYS_OBJ_2470();
    return uVar1;
  }
  do {
  } while ((GPU_REG1 & 0x4000000) == 0);
  (*unaff_s3)();
  DAT_8007e444 = unaff_s3;
  DAT_8007e448 = unaff_s0;
  DAT_8007e44c = unaff_s2;
  SetIntrMask(DAT_8007e45c);
  uVar1 = SYS_OBJ_2504();
  return uVar1;
}
