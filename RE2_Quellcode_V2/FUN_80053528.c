/* FUN_80053528 @ 0x80053528  (Ghidra headless, raw MIPS overlay) */

void FUN_80053528(void)

{
  undefined4 uVar1;
  int iVar2;
  
  if ((DAT_800cfb74 & 0x1000000) != 0) {
    uVar1 = FUN_800534c4((int)DAT_800d481c,(int)DAT_800d481e);
    iVar2 = FUN_80077360(&DAT_800d490c,uVar1);
    if (iVar2 == 0) {
      FUN_8007730c(&DAT_800d4854,0xff);
    }
    else {
      FUN_80077334(&DAT_800d4854,0xff);
    }
  }
  FUN_80010778(0x1f800000,&PTR_LAB_800a74c8,0x400);
  FUN_80052ca0();
  FUN_80052e20();
  FUN_80052f3c();
  DAT_800d8cc8 = 0;
  DAT_800d8cbc = *(undefined4 *)(DAT_800ce324 + 0x48);
  FUN_80053138(0,0);
  DAT_800d8cbc = *(undefined4 *)(DAT_800ce324 + 0x4c);
  FUN_80053138(1,0);
  FUN_800536c4();
  FUN_80052b38();
  FUN_80053244();
  FUN_800532d8();
  FUN_80053394();
  return;
}


