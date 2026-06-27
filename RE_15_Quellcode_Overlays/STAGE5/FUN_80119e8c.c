/* FUN_80119e8c @ 0x80119e8c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119e8c(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_8011a864();
        return;
      }
      *(short *)(_DAT_800ac784 + 0x6a) =
           *(short *)(_DAT_800ac784 + 0x6a) +
           (short)((int)((uint)*(byte *)(_DAT_800ac784 + 0x9e) << 0x18) >> 0x13);
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x20101;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x5a;
      return;
    }
    if (bVar1 != 0) {
      FUN_8011a864();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 8;
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + 0x400;
    sVar3 = func_0x8001a9cc(&DAT_800aca88,0x400);
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + -0x400;
    if (sVar3 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0xf8;
    }
  }
  *(short *)(_DAT_800ac784 + 0x6a) =
       (short)*(char *)(_DAT_800ac784 + 0x9e) + *(short *)(_DAT_800ac784 + 0x6a);
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  FUN_8011a864();
  return;
}


