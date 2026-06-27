undefined4 SYS_OBJ_2924(void)

{
  undefined4 uVar1;
  uint unaff_s0;
  
  SetIntrMask(DAT_8007e464);
  uVar1 = 0;
  if ((unaff_s0 & 7) == 0) {
    uVar1 = _version();
  }
  return uVar1;
}
