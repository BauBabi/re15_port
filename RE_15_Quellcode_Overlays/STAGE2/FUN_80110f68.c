/* FUN_80110f68 @ 0x80110f68  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110f68(void)

{
  uint uVar1;
  
  uVar1 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  else if (uVar1 != 1) {
    *(undefined2 *)(uVar1 + 0x1be) = 1;
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


