/* FUN_80114f7c @ 0x80114f7c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114f7c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801158d8();
        return;
      }
      *(byte *)(_DAT_800ac784 + 9) = *(byte *)(_DAT_800ac784 + 9) & 0xbf;
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
      return;
    }
    if (bVar1 != 0) {
      FUN_801158d8();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_801158d8();
  return;
}


