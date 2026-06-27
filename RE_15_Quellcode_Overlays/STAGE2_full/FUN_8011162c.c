/* FUN_8011162c @ 0x8011162c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011162c(void)

{
  short sVar1;
  ushort uVar2;
  uint uVar3;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) =
         (uVar2 & 0x3f) - ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) * 0x50 + -0x50);
  }
  else if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    uVar3 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 6) =
         *(undefined1 *)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) * 8 + -0x7fee715c + (uVar3 & 7));
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}


