undefined4 S_CRWA_OBJ_9C(void)

{
  int iVar1;
  undefined4 uVar2;
  uint unaff_s1;
  undefined4 unaff_s2;
  int unaff_s5;
  int in_stack_00000010;
  
  iVar1 = DAT_80076d74;
  if (DAT_80076d74 == 1) {
    DAT_80076d74 = 0;
    unaff_s5 = 1;
  }
  if (DAT_80076d90 != 0) {
    in_stack_00000010 = DAT_80076d90;
    DAT_80076d90 = 0;
  }
  if (unaff_s1 < 0x401) {
    _spu_t(2,unaff_s2);
    _spu_t(1);
    _spu_t(3,&DAT_80076e20,unaff_s1);
    WaitEvent(DAT_80076e18);
    if (unaff_s5 != 0) {
      DAT_80076d74 = iVar1;
    }
    if (in_stack_00000010 != 0) {
      DAT_80076d90 = in_stack_00000010;
    }
    return 0;
  }
  uVar2 = S_CRWA_OBJ_100();
  return uVar2;
}
