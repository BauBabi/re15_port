int FUN_800278bc(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = 10;
  iVar3 = 2;
  FUN_80027160();
  do {
    iVar1 = _card_clear(param_1 << 4);
    if ((iVar1 != 0) && (iVar3 = FUN_80027828(), iVar3 == 0)) break;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  printf("Card Clear Exit <%d>:\n",iVar3);
  return iVar3;
}
