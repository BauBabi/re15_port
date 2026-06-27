undefined4 FUN_80027828(void)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 249999;
  do {
    iVar1 = TestEvent(DAT_8008f66c);
    if (iVar1 != 0) {
      return 0;
    }
    iVar1 = TestEvent(DAT_8008f670);
    if (iVar1 != 0) {
      return 1;
    }
    iVar1 = TestEvent(DAT_8008f674);
    iVar2 = iVar2 + -1;
    if (iVar1 != 0) {
      return 2;
    }
  } while (iVar2 != 0);
  printf("Event2 loop time out\n");
  return 3;
}
