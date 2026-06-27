/* FUN_80100000 @ 0x80100000  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100000(void)

{
  int in_v0;
  
  *(undefined1 *)(in_v0 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x8001af5c(0,0,600,700);
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 4;
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  return;
}


