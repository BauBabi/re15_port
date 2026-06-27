/* FUN_801071b4 @ 0x801071b4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801071b4(void)

{
  char cVar1;
  int in_v0;
  
  *(undefined1 *)(in_v0 + 0x95) = 0;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_80109350(0,1);
  FUN_80107c50();
  return;
}


