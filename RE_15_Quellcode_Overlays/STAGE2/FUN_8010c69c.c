/* FUN_8010c69c @ 0x8010c69c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c69c(void)

{
  byte bVar1;
  ushort uVar2;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
      bVar1 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 2;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 0x3c;
  bVar1 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar1 & 0x1f) + 6;
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x800245d8(0);
  return;
}


