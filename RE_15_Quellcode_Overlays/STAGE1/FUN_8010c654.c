/* FUN_8010c654 @ 0x8010c654  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c654(void)

{
  char cVar1;
  byte bVar2;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 != 2) {
        FUN_8010cfc8();
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      if (*(char *)(_DAT_800ac784 + 0x9e) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x1d2) = (bVar2 & 3) + 1;
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      }
      return;
    }
    if (bVar2 != 0) {
      FUN_8010cfc8();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 1;
    if (*(char *)(_DAT_800ac784 + 5) == '\x04') {
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),
                          (int)*(char *)(_DAT_800ac784 + 0x9e),0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_8010cfc8();
  return;
}


