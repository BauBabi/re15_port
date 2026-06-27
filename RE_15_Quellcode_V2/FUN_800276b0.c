int FUN_800276b0(int param_1)

{
  int iVar1;
  int iVar2;
  int unaff_s1;
  
  iVar2 = 3;
  FUN_80027108();
  while ((iVar1 = func_0x8006e0c8(param_1 << 4), iVar1 == 0 ||
         (unaff_s1 = FUN_80027780(), unaff_s1 != 0))) {
    iVar2 = iVar2 + -1;
    if (iVar2 == 0) {
      return unaff_s1;
    }
  }
  return 0;
}
