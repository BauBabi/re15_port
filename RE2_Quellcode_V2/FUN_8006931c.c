/* FUN_8006931c @ 0x8006931c  (Ghidra headless, raw MIPS overlay) */

void FUN_8006931c(void)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = 0;
  switch(DAT_800d481c) {
  case 1:
    iVar2 = 0x1e;
    break;
  case 2:
    iVar2 = 0x3a;
    break;
  case 3:
    iVar2 = 0x48;
    break;
  case 4:
    iVar2 = 0x59;
    break;
  case 5:
    iVar2 = 99;
    break;
  case 6:
    iVar2 = 0x7b;
  }
  FUN_8007730c(&DAT_800d490c,iVar2 + DAT_800d481e);
  uVar1 = FUN_8006e7f0((int)DAT_800d481c,(int)DAT_800d481e);
  FUN_8007730c(&DAT_800d4908,uVar1);
  return;
}


