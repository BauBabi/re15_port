uint FUN_800275f0(int param_1)

{
  int iVar1;
  uint unaff_s0;
  int iVar2;
  uint uVar3;
  
  iVar2 = 0x10;
  uVar3 = 0xff;
  FUN_80027108();
  do {
    iVar1 = _card_info(param_1 << 4);
    if (iVar1 != 0) {
      unaff_s0 = FUN_80027780();
      if (((unaff_s0 & 1) == 0) && (unaff_s0 == uVar3)) break;
      printf("status:%d\n",unaff_s0);
      uVar3 = unaff_s0;
    }
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  if (unaff_s0 == 2) {
    FUN_800278bc(param_1);
    unaff_s0 = 0;
  }
  printf("Card check exit <%d>:\n",unaff_s0);
  return unaff_s0;
}
