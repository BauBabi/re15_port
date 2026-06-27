/* FUN_8011b7d8 @ 0x8011b7d8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011b7d8(void)

{
  char cVar1;
  undefined4 uVar2;
  short local_10 [4];
  
  cVar1 = func_0x800509e4(15000,local_10,0x800,0);
  if (cVar1 != '\0') {
    uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],-(int)local_10[0]);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
    if ((_DAT_000001c5 & 2) != 0) {
      func_0x800369f8(0,1);
    }
    return;
  }
  uVar2 = func_0x8005070c(0x5dc,(int)local_10[0],-(int)local_10[0]);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fee0a58))();
  return;
}


