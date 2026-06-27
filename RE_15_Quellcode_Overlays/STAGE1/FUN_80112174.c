/* FUN_80112174 @ 0x80112174  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112174(void)

{
  undefined1 uVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(0);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    FUN_80112a18();
    return;
  }
  uVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar1;
  if (*(char *)(_DAT_800ac784 + 0x1d3) != '\0') {
    func_0x800453d0(6);
  }
  return;
}


