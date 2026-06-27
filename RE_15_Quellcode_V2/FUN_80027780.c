undefined4 FUN_80027780(void)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 249999;
  while( true ) {
    iVar1 = TestEvent(DAT_8008f65c);
    if (iVar1 != 0) {
      return 0;
    }
    iVar1 = TestEvent(DAT_8008f660);
    if (iVar1 != 0) {
      return 1;
    }
    iVar1 = TestEvent(DAT_8008f664);
    if (iVar1 != 0) {
      return 2;
    }
    iVar1 = TestEvent(DAT_8008f668);
    if (iVar1 != 0) break;
    iVar2 = iVar2 + -1;
    if (iVar2 == 0) {
      printf("Event loop time out\n");
      return 3;
    }
  }
  return 3;
}
