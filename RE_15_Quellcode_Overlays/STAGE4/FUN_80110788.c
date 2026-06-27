/* FUN_80110788 @ 0x80110788  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110788(void)

{
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 1;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4 + -0x7fee6188);
  *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) & 0xbf;
  *(undefined1 *)(_DAT_800ac784 + 4) = 1;
  *(undefined1 *)(_DAT_800ac784 + 5) = 10;
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x13;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  return;
}


