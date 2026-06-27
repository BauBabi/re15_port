/* FUN_80109234 @ 0x80109234  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109234(void)

{
  ushort uVar1;
  
  if ((*(ushort *)(_DAT_800ac784 + 0x1d8) & 0x100) != 0) {
    uVar1 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar1 & 0xff) + 100;
    FUN_80109ea8();
    return;
  }
  return;
}


