undefined4 FUN_80029cd8(void)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  
  FUN_8002ac84(100);
  FUN_8002ad34(1);
  SsSetSerialAttr('\0','\0','\x01');
  bVar1 = false;
  FUN_8002a16c(0x140,0);
  uVar3 = 0;
  uRam0000001c = 1;
  do {
    iVar2 = FUN_8002a2a8(0,0);
    if (iVar2 == 0) {
      FUN_80030444();
      do {
        iVar2 = FUN_8002a630(0);
        FUN_80029f40(uRam00000000);
        FUN_8002a1b8();
        FUN_80030444();
        if ((DAT_800ac762 & 0x800) != 0) {
          bVar1 = true;
        }
        if ((bVar1) && (DAT_800aca34 == '\0')) {
          iVar2 = 1;
        }
      } while (iVar2 == 0);
      FUN_8002a8d0();
      return 1;
    }
    uVar3 = uVar3 + 1;
    FUN_8002a8d0();
  } while (uVar3 < 4);
  return 0;
}
