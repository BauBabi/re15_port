int TMD_OBJ_448(int param_1)

{
  uint uVar1;
  uint *unaff_s0;
  int unaff_s1;
  
  uVar1 = *unaff_s0;
  *(uint **)(unaff_s1 + 0xc) = unaff_s0 + 1;
  *(uint **)(unaff_s1 + 0x10) = unaff_s0 + 3;
  return param_1 + (uVar1 >> 2) + 2;
}
