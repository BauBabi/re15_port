undefined4 S_CRWA_OBJ_100(void)

{
  undefined4 uVar1;
  uint unaff_s0;
  uint unaff_s1;
  int unaff_s2;
  int unaff_s3;
  undefined4 unaff_s4;
  int unaff_s5;
  int in_stack_00000010;
  
  while( true ) {
    _spu_t(2,unaff_s2);
    _spu_t(1);
    _spu_t(3,&DAT_80076e20,unaff_s0);
    unaff_s0 = unaff_s1 - 0x400;
    unaff_s2 = unaff_s2 + 0x400;
    WaitEvent(DAT_80076e18);
    if (unaff_s3 == 0) {
      if (unaff_s5 != 0) {
        DAT_80076d74 = unaff_s4;
      }
      if (in_stack_00000010 != 0) {
        DAT_80076d90 = in_stack_00000010;
      }
      return 0;
    }
    if (0x400 < unaff_s0) break;
    unaff_s3 = 0;
    unaff_s1 = unaff_s0;
  }
  uVar1 = S_CRWA_OBJ_100();
  return uVar1;
}
