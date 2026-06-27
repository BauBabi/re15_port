/* FUN_80104f80 @ 0x80104f80  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104f80(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 9) = 0;
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
      *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 4;
      if (*(char *)(_DAT_800ac784 + 0x9f) != '\0') {
        *(undefined1 *)(_DAT_800ac784 + 9) = 0x81;
        *(undefined4 *)(_DAT_800ac784 + 4) = 1;
        *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 8;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 1;
    }
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),
                          (int)*(char *)(_DAT_800ac784 + 0x9f),0x100);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  return;
}


