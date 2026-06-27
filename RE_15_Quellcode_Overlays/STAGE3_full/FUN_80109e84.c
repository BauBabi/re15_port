/* FUN_80109e84 @ 0x80109e84  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109e84(void)

{
  byte bVar1;
  int iVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = bVar1 & 1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x180),
                          *(undefined4 *)(_DAT_800ac784 + 0x184),0,0x200);
  if (iVar2 != 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


