/* FUN_8011d388 @ 0x8011d388  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011d388(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x514,0);
  if (cVar1 == '\0') {
    uVar2 = func_0x8005070c(700,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  else {
    uVar2 = func_0x8005070c(0x578,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
  }
  if (((*(byte *)(_DAT_800ac784 + 0x1c2) & 1) == 0) || (_DAT_800acae0 < 0x65)) {
    (*(code *)(&PTR_FUN_8012070c)[*(byte *)(_DAT_800ac784 + 9) & 0xf])();
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 4) = 2;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


