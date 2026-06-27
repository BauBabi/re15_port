/* FUN_8010e2d0 @ 0x8010e2d0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e2d0(void)

{
  char cVar1;
  int iVar2;
  
  cVar1 = func_0x8001f314();
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (*(char *)(_DAT_800ac784 + 6) == '\x06') {
    func_0x800453d0(7);
    iVar2 = func_0x8001f314();
    if (iVar2 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    return;
  }
  return;
}


