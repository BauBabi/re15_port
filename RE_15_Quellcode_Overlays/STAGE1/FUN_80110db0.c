/* FUN_80110db0 @ 0x80110db0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110db0(undefined4 param_1,undefined4 param_2)

{
  int in_v0;
  
  func_0x8001f314(param_1,param_2,*(undefined1 *)(in_v0 + 7),0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 5;
    FUN_80111648();
    return;
  }
  return;
}


