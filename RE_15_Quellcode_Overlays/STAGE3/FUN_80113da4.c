/* FUN_80113da4 @ 0x80113da4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113da4(void)

{
  char cVar1;
  
  func_0x800453d0();
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_80114780();
  return;
}


